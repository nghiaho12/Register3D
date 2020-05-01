#include "MyANN.h"
#include "ICPPoint.h"
#include "Point.h"

ANN::ANN()
{
    m_anything_to_free = false;
    m_eps = 0;
    m_k = 1;
}

ANN::~ANN() { Free(); }

void ANN::Free()
{
    if (!m_anything_to_free)
        return;

    delete[] m_nnidx; // clean things up
    delete[] m_dists;
    delete m_kdtree;

    annDeallocPts(m_data_pts);

    annClose(); // done with ANN

    m_anything_to_free = false;
}

void ANN::SetPoints(vector<ICPPoint>& P)
{
    Free(); // Free, if theres anything to free

    m_nnidx = new ANNidx[m_k]; // allocate near neigh indices
    m_dists = new ANNdist[m_k]; // allocate near neighbor m_dists
    m_query_pt = annAllocPt(dim); // allocate query point

    m_npts = P.size();

    m_data_pts = annAllocPts(P.size(), dim); // allocate data points

    for (unsigned int i = 0; i < P.size(); i++) {
        m_data_pts[i][0] = P[i].x;
        m_data_pts[i][1] = P[i].y;
        m_data_pts[i][2] = P[i].z;
    }

    m_kdtree = new ANNkd_tree( // build search structure
        m_data_pts, // the data points
        m_npts, // number of points
        dim); // dimension of space

    m_anything_to_free = true;
}

void ANN::SetPoints(vector<Point>& P)
{
    Free(); // Free, if theres anything to free

    m_nnidx = new ANNidx[m_k]; // allocate near neigh indices
    m_dists = new ANNdist[m_k]; // allocate near neighbor m_dists
    m_query_pt = annAllocPt(dim); // allocate query point

    m_npts = P.size();

    m_data_pts = annAllocPts(P.size(), dim); // allocate data points

    for (unsigned int i = 0; i < P.size(); i++) {
        m_data_pts[i][0] = P[i].x;
        m_data_pts[i][1] = P[i].y;
        m_data_pts[i][2] = P[i].z;
    }

    m_kdtree = new ANNkd_tree( // build search structure
        m_data_pts, // the data points
        m_npts, // number of points
        dim); // dimension of space

    m_anything_to_free = true;
}

void ANN::FindClosest(Point& P, double* sq_distance, int* index)
{
    m_query_pt[0] = P.x;
    m_query_pt[1] = P.y;
    m_query_pt[2] = P.z;

    m_kdtree->annkSearch( // search
        m_query_pt, // query point
        m_k, // number of near neighbors
        m_nnidx, // nearest neighbors (returned)
        m_dists, // distance (returned)
        m_eps); // error bound

    for (int i = 0; i < m_k; i++) {
        sq_distance[i] = m_dists[i];
        index[i] = m_nnidx[i];
    }
}

void ANN::FindClosest(Point& P, double& sq_distance, int& index)
{
    m_query_pt[0] = P.x;
    m_query_pt[1] = P.y;
    m_query_pt[2] = P.z;

    m_kdtree->annkSearch( // search
        m_query_pt, // query point
        m_k, // number of near neighbors
        m_nnidx, // nearest neighbors (returned)
        m_dists, // distance (returned)
        m_eps); // error bound

    sq_distance = m_dists[0];
    index = m_nnidx[0];
}

void ANN::Seteps(float e) { m_eps = e; }

void ANN::SetResults(int a) { m_k = a; }
