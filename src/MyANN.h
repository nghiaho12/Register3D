#pragma once

/*
My wrapper around ANN
*/

#include "ANN/ANN.h"
#include "ICPPoint.h"
#include "Point.h"
#include <vector>

class ANN {
public:
    ANN();
    ~ANN();
    void Free();
    void SetPoints(std::vector<ICPPoint>& P);
    void SetPoints(std::vector<Point>& P);
    void FindClosest(Point& P, double& sq_distance, int& index);
    void FindClosest(Point& P, double* sq_distance, int* index);
    void Seteps(float e);
    void SetResults(int a);

private:
    int m_npts; // actual number of data points
    ANNpointArray m_data_pts; // data points
    ANNpoint m_query_pt; // query point
    ANNidxArray m_nnidx; // near neighbor indices
    ANNdistArray m_dists; // near neighbor distances
    ANNkd_tree* m_kdtree; // search structure

    static const int dim = 3;
    int m_k;
    double m_eps;
    bool m_anything_to_free;
};
