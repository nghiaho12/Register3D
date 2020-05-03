#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <chrono>

#include <Eigen/SVD>

#include "ICP.h"
#include "ICPPoint.h"
#include "Misc.h"
#include "Point.h"
#include "PointOP.h"

ICP::ICP(SharedData &shared_data) :
    m_shared_data(shared_data)
{
    m_LTS = 1.0;
    m_text = NULL;
    m_max_points = 0;
}

void ICP::SetLTS(float a) { m_LTS = a; }

void ICP::Run(Eigen::Matrix4d& Transform)
{
    // Some distance statistics
    std::vector<double> dist_sq;

    auto update_text = [&]() {
        while (m_app->Pending()) {
            m_app->Dispatch();
        }
    };

    if (m_text) {
        m_text->AppendText("Determining square distance to trim at ...\n");
        update_text();
    }

    for (size_t i = 0; i < m_points1.size(); i++) {
        if (i % 100000 == 0) {
            if (m_text) {
                std::stringstream ss;

                ss << i * 100 / (float)m_points1.size() << "% ";
                m_text->AppendText(ss.str());

                update_text();
            }
        }

        double dist;
        int index;

        m_ANN_points2.FindClosest(m_points1[i], dist, index);

        m_points1[i].nearest = m_points2[index];
        m_points1[i].dist_sq = dist;

        dist_sq.push_back(dist);
    }

    if (m_text) {
        m_text->AppendText("100%\n");
        update_text();
    }

    std::sort(dist_sq.begin(), dist_sq.end());

    size_t pos = (int)(dist_sq.size() * m_LTS);

    if (pos >= dist_sq.size()) {
        pos = dist_sq.size() - 1;
    }

    double trimDist = dist_sq[pos];

    if (m_text) {
        std::stringstream ss;
        ss << "Trimming at distance: " << trimDist << "\n";
        m_text->AppendText(ss.str());
        update_text();
    }

    std::vector<ICPPoint> Points2p;

    // Find which points to use based on trimDist and also finds the centroid of
    // the two points
    Point centroid1, centroid2;

    centroid1.x = 0;
    centroid1.y = 0;
    centroid1.z = 0;

    centroid2.x = 0;
    centroid2.y = 0;
    centroid2.z = 0;

    double sum_dist = 0;
    int count = 0;

    for (size_t i = 0; i < m_points1.size(); i++) {
        if (m_points1[i].dist_sq > trimDist) {
            continue;
        }

        sum_dist += m_points1[i].dist_sq;

        centroid1.x += m_points1[i].x;
        centroid1.y += m_points1[i].y;
        centroid1.z += m_points1[i].z;

        centroid2.x += m_points1[i].nearest.x;
        centroid2.y += m_points1[i].nearest.y;
        centroid2.z += m_points1[i].nearest.z;

        count++;
    }

    m_MSE = sum_dist / count;

    centroid1.x /= count;
    centroid1.y /= count;
    centroid1.z /= count;

    centroid2.x /= count;
    centroid2.y /= count;
    centroid2.z /= count;

    /************************************
   * FIND OPTIMAL ROTATION USING SVD
   ************************************/

    Eigen::Matrix3d H;

    H.setZero();

    if (m_text) {
        m_text->AppendText("Calculating optimal transform ... \n");
        update_text();
    }
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    for (size_t i = 0; i < m_points1.size(); i++) {
        if (m_points1[i].dist_sq > trimDist) {
            continue;
        }

        Eigen::Vector3d A, B;

        A(0) = m_points1[i].x - centroid1.x;
        A(1) = m_points1[i].y - centroid1.y;
        A(2) = m_points1[i].z - centroid1.z;

        B(0) = m_points1[i].nearest.x - centroid2.x;
        B(1) = m_points1[i].nearest.y - centroid2.y;
        B(2) = m_points1[i].nearest.z - centroid2.z;

        H += A * B.transpose();
    }

    Eigen::JacobiSVD<Eigen::Matrix3d> svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);

    // Optimal rotation
    Eigen::Matrix3d R = svd.matrixV() * svd.matrixU().transpose();

    // Final transformation matrix is T*R*Tc
    // Tc - translates point1 to the centre for finding the rotation
    // R - optimal rotation
    // T - translates point1 back to the centre of point2

    Eigen::Matrix4d Tc, T, RR;

    Tc.setIdentity();
    T.setIdentity();
    RR.setIdentity();

    Tc(0, 3) = -centroid1.x;
    Tc(1, 3) = -centroid1.y;
    Tc(2, 3) = -centroid1.z;

    T(0, 3) = centroid2.x;
    T(1, 3) = centroid2.y;
    T(2, 3) = centroid2.z;

    RR.block(0,0,3,3) = R;

    Transform = T * (RR * Tc);

    PointOP::ApplyTransform(m_points1, Transform);

    // Clean up memory
    {
        std::vector<ICPPoint> Empty;
        Points2p.swap(Empty);
    }
}

void ICP::SetMaxPoints(unsigned int max) { m_max_points = max; }

void ICP::SetPoints(std::vector<Point>& P1, std::vector<Point>& P2,
    float dist_threshold)
{
    const float sq_dist = dist_threshold * dist_threshold;

    if (m_max_points == 0) {
        throw std::runtime_error("ICP: Need to set m_max_points before running SetPoints");
    }

    ANN Point1DB, Point2DB;
    std::vector<Point> filtered1, filtered2;
    Point start, end;

    reverseable_shuffle_forward(P1, m_shared_data.table[0]);
    reverseable_shuffle_forward(P2, m_shared_data.table[1]);

    auto update_text = [&]() {
        while (m_app->Pending()) {
            m_app->Dispatch();
        }
    };

    // NOTE:
    // The aim of this is to save as much memory as possible.
    // We downsample the points before loading it into the ANN library. At the
    // same time we want to avoid over downsampling. We want the process result
    // in: point cloud2 -> overlapping region filtering -> down sample > initial outlier
    // distance -> max point filtering = number of points point cloud2 is m_max_points So
    // basically, we want all the filtering and downsampling stuff to leave enough
    // points to meet the user's requested MaxPoint parameter if possible

    // Downsample based on a factor of m_max_points
    size_t k = m_max_points * 2;

    if (P1.size() > k) {
        filtered1.resize(k);
    } else {
        filtered1.resize(P1.size());
    }

    for (size_t i = 0; i < filtered1.size(); i++) {
        filtered1[i] = P1[i];
    }

    if (P2.size() > k) {
        filtered2.resize(k);
    } else {
        filtered2.resize(P2.size());
    }

    for (size_t i = 0; i < filtered2.size(); i++) {
        filtered2[i] = P2[i];
    }

    // This consumes the most memory out of the entire program
    // Every 1 million point uses about 200MB of memory when loaded using ANN
    Point2DB.SetPoints(filtered2);

    // for displaying purposes
    if (m_text) {
        m_text->AppendText("Downsampling and filtering first point cloud for ICP ... \n");
    }

    for (size_t i = 0;
         i < filtered1.size() && m_points1.size() < m_max_points; i++) {
        if (i % 100000 == 0) {
            if (m_text) {
                std::stringstream ss;

                ss << i * 100 / (float)filtered1.size() << "% ";
                m_text->AppendText(ss.str());

                update_text();
            }
        }

        double dist;
        int index;

        Point2DB.FindClosest(filtered1[i], dist, index);

        if (dist < sq_dist) {
            m_points1.push_back(ICPPoint(filtered1[i]));
        }
    }

    if (m_text) {
        m_text->AppendText("100%\n");

        std::stringstream ss;
        ss << "Number of points after downsampling/filtering: " << m_points1.size() << "\n";
        m_text->AppendText(ss.str());

        update_text();
    }

    Point2DB.Free();

    {
        std::vector<Point> empty;
        filtered1.swap(empty);
    }

    Point1DB.SetPoints(m_points1);

    if (m_text) {
        m_text->AppendText("Downsampling and filtering second point cloud for ICP ... \n");
    }

    // Do the same again

    for (size_t i = 0;
         i < filtered2.size() && m_points2.size() < m_max_points; i++) {
        if (i % 100000 == 0) {
            if (m_text) {
                std::stringstream ss;
                ss << i * 100 / (float)filtered2.size() << "% ";
                m_text->AppendText(ss.str());

                update_text();
            }
        }

        double dist;
        int index;

        Point1DB.FindClosest(filtered2[i], dist, index);

        if (dist < sq_dist) {
            m_points2.push_back(filtered2[i]);
        }
    }

    if (m_text) {
        m_text->AppendText("100%\n");

        std::stringstream ss;
        ss << "Number of points after downsampling/filtering: " << m_points2.size() << "\n";
        m_text->AppendText(ss.str());

        update_text();
    }

    Point1DB.Free();

    m_ANN_points2.SetPoints(m_points2);

    reverseable_shuffle_backward(P1, m_shared_data.table[0]);
    reverseable_shuffle_backward(P2, m_shared_data.table[1]);
}

void ICP::Seteps(float e)
{
    m_eps = e;
    m_ANN_points2.Seteps(m_eps);
}

double ICP::GetMSE() { return m_MSE; }
void ICP::SetwxTextCtrl(wxTextCtrl* t) // Used for m_text feedback from ICP
{
    m_text = t;
}
void ICP::SetwxApp(wxApp* a) // Used for m_text feedback from ICP
{
    m_app = a;
}
