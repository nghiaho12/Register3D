#pragma once

#include <vector>
#include <Eigen/Dense>

#include "ICPPoint.h"
#include "Point.h"

class PointOP {
    // Point operation
public:
    template <typename TYPE>
    static void ApplyTransform(std::vector<TYPE>& P, Eigen::Matrix4d& T) {
        for (size_t i = 0; i < P.size(); i++) {
            float x = P[i].x;
            float y = P[i].y;
            float z = P[i].z;

            float new_x = x * T(0, 0) + y * T(0, 1) + z * T(0, 2) + T(0, 3);
            float new_y = x * T(1, 0) + y * T(1, 1) + z * T(1, 2) + T(1, 3);
            float new_z = x * T(2, 0) + y * T(2, 1) + z * T(2, 2) + T(2, 3);

            P[i].x = new_x;
            P[i].y = new_y;
            P[i].z = new_z;
        }
    }

    static Eigen::Matrix4d GetTransform(const std::vector<Point> &regpoint1, const std::vector<Point> &regpoint2);
};
