#include "ICP.h"
#include <algorithm>
#include <iostream>
#include <numeric>

#include "Global.h"
#include "ICPPoint.h"
#include "Matrix.h"
#include "Misc.h"
#include "Point.h"
#include "PointOP.h"

using namespace std;

extern _Global Global;

ICP::ICP()
{
    m_LTS = 1.0;
    m_text = NULL;
    m_max_points = 0;
}

void ICP::SetLTS(float a) { m_LTS = a; }

/*
void ICP::SetPoints(vector <Point> &P1, vector <Point> &P2)
{
    m_points1.clear();
        Global.scan2.clear();

        for(unsigned int i=0; i < P1.size(); i++)
                m_points1.push_back(ICPPoint(P1[i]));
}
*/

bool ICP::ByDistSq(const ICPPoint& a, const ICPPoint& b)
{
    return a.dist_sq < b.dist_sq;
}

void ICP::Run(Matrix& Transform)
{
    // Make sure Transform Matrix is 4x4
    if (Transform.GetRow() != 4 || Transform.GetCol() != 4) {
        fprintf(stderr, "Not a 4x4 matrix\n");
        exit(1);
    }

    // Some distance statistics
    vector<double> DistSq;

    if (m_text) {
        m_text->AppendText(wxT("Determining square distance to trim at ... "));

        while (m_app->Pending())
            m_app->Dispatch();
    }

    for (unsigned int i = 0; i < m_points1.size(); i++) {
        if (i % 100000 == 0) {
            if (m_text) {
                m_text->AppendText(wxString::Format(wxT("%.0f%% "),
                    i * 100 / (float)m_points1.size()));

                while (m_app->Pending())
                    m_app->Dispatch();
            }
        }

        double dist;
        int index;

        m_ANN_points2.FindClosest(m_points1[i], dist, index);

        m_points1[i].nearest = m_points2[index];
        m_points1[i].dist_sq = dist;

        DistSq.push_back(dist);
    }

    if (m_text) {
        m_text->AppendText(wxT("100%\n"));

        while (m_app->Pending())
            m_app->Dispatch();
    }

    sort(DistSq.begin(), DistSq.end());

    unsigned int pos = (int)(DistSq.size() * m_LTS);

    if (pos >= DistSq.size())
        pos = DistSq.size() - 1;

    double trimDist = DistSq[pos];

    if (m_text) {
        m_text->AppendText(
            wxString::Format(wxT("Trimming at distance: %e\n"), trimDist));

        while (m_app->Pending())
            m_app->Dispatch();
    }

    // End stat

    vector<ICPPoint> Points2p;

    if (m_text) {
        m_text->AppendText(wxT("Trimming points ... "));

        while (m_app->Pending())
            m_app->Dispatch();
    }

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

    for (unsigned int i = 0; i < m_points1.size(); i++) {
        if (i % 100000 == 0) {
            if (m_text) {
                m_text->AppendText(wxString::Format(wxT("%.0f%% "),
                    i * 100 / (float)m_points1.size()));

                while (m_app->Pending())
                    m_app->Dispatch();
            }
        }

        if (m_points1[i].dist_sq > trimDist)
            continue;

        // Points2p.push_back(m_points1[i].nearest);
        sum_dist += m_points1[i].dist_sq;

        centroid1.x += m_points1[i].x;
        centroid1.y += m_points1[i].y;
        centroid1.z += m_points1[i].z;

        centroid2.x += m_points1[i].nearest.x;
        centroid2.y += m_points1[i].nearest.y;
        centroid2.z += m_points1[i].nearest.z;

        count++;
    }

    if (m_text) {
        m_text->AppendText(wxT("100%\n"));

        while (m_app->Pending())
            m_app->Dispatch();
    }

    m_MSE = sum_dist / count;

    // printf("m_MSE: %.10f\n", m_MSE);
    centroid1.x /= count;
    centroid1.y /= count;
    centroid1.z /= count;

    centroid2.x /= count;
    centroid2.y /= count;
    centroid2.z /= count;

    cout << "CENTROIDS: " << centroid1.x << " " << centroid1.y << " "
         << centroid1.z << " --- " << centroid2.x << " " << centroid2.y << " "
         << centroid2.z << endl;

    /*
          ANN ANN_points2p;

          for(unsigned int i=0; i < Points2p.size(); i++)
          {
                  Points2p[i].x -= centroid2.x;
                  Points2p[i].y -= centroid2.y;
                  Points2p[i].z -= centroid2.z;
          }

          ANN_points2p.SetPoints(Points2p);
  */
    /************************************
   * FIND OPTIMAL ROTATION USING SVD
   ************************************/

    Matrix H(3, 3), A(3, 1), B(1, 3), C(3, 3), R(3, 3);
    Matrix U(3, 3), S(3, 3), V(3, 3); // SVD

    A.Zero();
    B.Zero();

    H.LoadIdentity();

    if (m_text) {
        m_text->AppendText(wxT("Calculating optimal rotation ... "));

        while (m_app->Pending())
            m_app->Dispatch();
    }

    for (unsigned int i = 0; i < m_points1.size(); i++) {
        if (i % 100000 == 0) {
            if (m_text) {
                m_text->AppendText(wxString::Format(wxT("%.0f%% "),
                    i * 100 / (float)m_points1.size()));

                while (m_app->Pending())
                    m_app->Dispatch();
            }
        }

        if (m_points1[i].dist_sq > trimDist)
            continue;

        // double dist;
        // int index;

        // ANN_points2p.FindClosest(P, dist, index);

        // cout << (m_points1[i].x - centroid1.x) << " ---- " <<
        // (m_points1[i].nearest.x - centroid2.x) << endl;
        A.Set(0, 0, m_points1[i].x - centroid1.x);
        A.Set(1, 0, m_points1[i].y - centroid1.y);
        A.Set(2, 0, m_points1[i].z - centroid1.z);

        B.Set(0, 0, m_points1[i].nearest.x - centroid2.x);
        B.Set(0, 1, m_points1[i].nearest.y - centroid2.y);
        B.Set(0, 2, m_points1[i].nearest.z - centroid2.z);

        C = A * B;

        H += C;
    }

    if (m_text) {
        m_text->AppendText(wxT("100%\n"));

        while (m_app->Pending())
            m_app->Dispatch();
    }

    H.SVD(U, S, V);

    // Optimal rotation
    R = V * U.Transpose();

    // Final transformation matrix is T*R*Tc
    // Tc - translates point1 to the centre for finding the rotation
    // R - optimal rotation
    // T - translates point1 back to the centre of point2

    Matrix Tc(4, 4), T(4, 4), RR(4, 4);

    Tc.LoadIdentity();
    T.LoadIdentity();

    Tc.Set(0, 3, -centroid1.x);
    Tc.Set(1, 3, -centroid1.y);
    Tc.Set(2, 3, -centroid1.z);

    T.Set(0, 3, centroid2.x);
    T.Set(1, 3, centroid2.y);
    T.Set(2, 3, centroid2.z);

    RR.LoadIdentity();

    RR.Set(0, 0, R.Get(0, 0));
    RR.Set(0, 1, R.Get(0, 1));
    RR.Set(0, 2, R.Get(0, 2));

    RR.Set(1, 0, R.Get(1, 0));
    RR.Set(1, 1, R.Get(1, 1));
    RR.Set(1, 2, R.Get(1, 2));

    RR.Set(2, 0, R.Get(2, 0));
    RR.Set(2, 1, R.Get(2, 1));
    RR.Set(2, 2, R.Get(2, 2));

    Transform = T * (RR * Tc);

    PointOP::ApplyTransform(m_points1, Transform);

    // Transform.Print();

    // Clean up memory
    {
        vector<ICPPoint> Empty;
        Points2p.swap(Empty);
    }
}

void ICP::SetMaxPoints(unsigned int max) { m_max_points = max; }

void ICP::SetPoints(vector<Point>& P1, vector<Point>& P2,
    float dist_threshold)
{
    const float sq_dist = dist_threshold * dist_threshold;

    if (m_max_points == 0) {
        cerr << "ICP: Need to set m_max_points before running SetPoints" << endl;
        exit(1);
    }

    ANN Point1DB, Point2DB;
    vector<Point> filtered1, filtered2;
    Point start, end;

    reverseable_shuffle_forward(P1, Global.table1);
    reverseable_shuffle_forward(P2, Global.table2);

    // EXPERIMENTAL:
    // The aim of this is to save as much memory as possible.
    // We downsample the points before loading it into the ANN library. At the
    // same time we want to avoid over downsampling. We want the process result
    // in: scan2 -> overlapping region filtering -> down sample > initial outlier
    // distance -> max point filtering = number of points scan2 is m_max_points So
    // basically, we want all the filtering and downsampling stuff to leave enough
    // points to meet the user's requested MaxPoint parameter if possible

    // Downsample based on a factor of m_max_points
    unsigned int k = m_max_points * 2;

    if (P1.size() > k)
        filtered1.resize(k);
    else
        filtered1.resize(P1.size());

    for (unsigned int i = 0; i < filtered1.size(); i++)
        filtered1[i] = P1[i];

    if (P2.size() > k)
        filtered2.resize(k);
    else
        filtered2.resize(P2.size());

    for (unsigned int i = 0; i < filtered2.size(); i++)
        filtered2[i] = P2[i];

    // This consumes the most memory out of the entire program
    // Every 1 million point uses about 200MB of memory when loaded using ANN
    Point2DB.SetPoints(filtered2);

    // for displaying purposes
    if (m_text)
        m_text->AppendText(
            wxT("Downsampling and filtering first scan for ICP ... "));

    for (unsigned int i = 0;
         i < filtered1.size() && m_points1.size() < m_max_points; i++) {
        if (i % 100000 == 0) {
            if (m_text) {
                m_text->AppendText(wxString::Format(wxT("%.0f%% "),
                    i * 100 / (float)filtered1.size()));

                while (m_app->Pending())
                    m_app->Dispatch();
            }
        }

        double dist;
        int index;

        Point2DB.FindClosest(filtered1[i], dist, index);

        if (dist < sq_dist)
            m_points1.push_back(ICPPoint(filtered1[i]));
    }

    if (m_text) {
        m_text->AppendText(wxT("100%\n"));
        m_text->AppendText(wxString::Format(
            wxT("Number of points after downsampling/filtering: %d\n"),
            m_points1.size()));

        while (m_app->Pending())
            m_app->Dispatch();
    }

    Point2DB.Free();

    {
        vector<Point> empty;
        filtered1.swap(empty);
    }

    Point1DB.SetPoints(m_points1);

    if (m_text)
        m_text->AppendText(
            wxT("Downsampling and filtering second scan for ICP ... "));

    // Do the same again

    for (unsigned int i = 0;
         i < filtered2.size() && m_points2.size() < m_max_points; i++) {
        if (i % 100000 == 0) {
            if (m_text) {
                m_text->AppendText(wxString::Format(wxT("%.0f%% "),
                    i * 100 / (float)filtered2.size()));

                while (m_app->Pending())
                    m_app->Dispatch();
            }
        }

        double dist;
        int index;

        Point1DB.FindClosest(filtered2[i], dist, index);

        if (dist < sq_dist)
            m_points2.push_back(filtered2[i]);
    }

    std::cout << "Number of points after downsampling: " << m_points2.size() << "\n";

    if (m_text) {
        m_text->AppendText(wxT("100%\n"));
        m_text->AppendText(wxString::Format(
            wxT("Number of points after downsampling/filtering: %d\n"),
            m_points2.size()));

        while (m_app->Pending()) {
            m_app->Dispatch();
        }
    }

    Point1DB.Free();

    m_ANN_points2.SetPoints(m_points2);

    reverseable_shuffle_backward(P1, Global.table1);
    reverseable_shuffle_backward(P2, Global.table2);
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

void ICP::CalcOverlappingRegion(vector<Point>& P1, vector<Point>& P2,
    Point& start, Point& end)
{
    // Calculates the overlapping region of the bounding box around thw two point
    // cloud

    // First calculate the bounding boxes
    Point box1_start, box2_start;
    Point box1_end, box2_end;

    box1_start.x = 0;
    box1_start.y = 0;
    box1_start.z = 0;

    box2_start = box1_start;
    box1_end = box1_start;
    box2_end = box1_start;

    for (unsigned int i = 0; i < P1.size(); i++) {
        if (P1[i].x < box1_start.x)
            box1_start.x = P1[i].x;
        if (P1[i].y < box1_start.y)
            box1_start.y = P1[i].y;
        if (P1[i].z < box1_start.z)
            box1_start.z = P1[i].z;

        if (P1[i].x > box1_end.x)
            box1_end.x = P1[i].x;
        if (P1[i].y > box1_end.y)
            box1_end.y = P1[i].y;
        if (P1[i].z > box1_end.z)
            box1_end.z = P1[i].z;
    }

    for (unsigned int i = 0; i < P2.size(); i++) {
        if (P2[i].x < box2_start.x)
            box2_start.x = P2[i].x;
        if (P2[i].y < box2_start.y)
            box2_start.y = P2[i].y;
        if (P2[i].z < box2_start.z)
            box2_start.z = P2[i].z;

        if (P2[i].x > box2_end.x)
            box2_end.x = P2[i].x;
        if (P2[i].y > box2_end.y)
            box2_end.y = P2[i].y;
        if (P2[i].z > box2_end.z)
            box2_end.z = P2[i].z;
    }

    vector<float> s;

    // x
    s.push_back(box1_start.x);
    s.push_back(box1_end.x);
    s.push_back(box2_start.x);
    s.push_back(box2_end.x);

    sort(s.begin(), s.end());

    start.x = s[1];
    end.x = s[2];

    // y
    s.clear();
    s.push_back(box1_start.y);
    s.push_back(box1_end.y);
    s.push_back(box2_start.y);
    s.push_back(box2_end.y);

    sort(s.begin(), s.end());

    start.y = s[1];
    end.y = s[2];

    // z
    s.clear();
    s.push_back(box1_start.z);
    s.push_back(box1_end.z);
    s.push_back(box2_start.z);
    s.push_back(box2_end.z);

    sort(s.begin(), s.end());

    start.z = s[1];
    end.z = s[2];
}
