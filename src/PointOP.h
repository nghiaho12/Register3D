#pragma once

#include <vector>
#include <Eigen/Dense>

#include "ICPPoint.h"
#include "Point.h"

class PointOP {
    // Point operation
public:
    static void ApplyTransform(std::vector<Point>& P, Eigen::Matrix4d& T);
    static void ApplyTransform(std::vector<ICPPoint>& P, Eigen::Matrix4d& T);
    static void GetTransform(std::vector<Point> regpoint1, std::vector<Point> regpoint2,
        Eigen::Matrix4d& transform);
    static void GetTransform2(std::vector<Point>& regpoint1, std::vector<Point>& regpoint2,
        Eigen::Matrix4d& transform);
};
