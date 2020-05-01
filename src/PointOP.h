#pragma once

#include "ICPPoint.h"
#include "Matrix.h"
#include "Point.h"
#include <vector>

class PointOP {
    // Point operation
public:
    static void ApplyTransform(std::vector<Point>& P, Matrix& T);
    static void ApplyTransform(std::vector<ICPPoint>& P, Matrix& T);
    static void GetTransform(std::vector<Point> regpoint1, std::vector<Point> regpoint2,
        Matrix& transform);
    static void GetTransform2(std::vector<Point>& regpoint1, std::vector<Point>& regpoint2,
        Matrix& transform);
};
