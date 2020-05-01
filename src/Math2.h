#pragma once

// My own maths routine
#include "Point.h"
#include <vector>

class Math2 {
public:
    static void ApplyRotation(Point& P, double angle_x, double angle_y,
        double angle_z);
    static void ApplyRotation(std::vector<Point>& points, double angle_x,
        double angle_y, double angle_z);
    static void RotatePoint(Point& P, float angle, float x, float y,
        float z); // Copy of glRotatef
    static void RotatePoints(std::vector<Point>& points, float angle, float x, float y,
        float z); // Copy of glRotatef

    // Principal Component Analysis
    static void PCA(const std::vector<Point>& points, Point& PC1, Point& PC2,
        Point& PC3, Point& Mean);
    static void PlaneRegression(const std::vector<Point>& points, double& A, double& B,
        double& C, double& D);

    // Lines, Planes
    static float Magnitude(Point& Point1, Point& Point2);
    static float Magnitude(Point& P);
    static float DistancePointLine(Point& P, Point& line_start, Point& line_end);
    static void DistancePointLine(Point& P, Point& line_start, Point& line_end,
        float& distance, float& u, Point& DirVector);
    static float DistancePointPlane(Point& P, float a, float b, float c, float d);

    static bool PlaneEquation(Point& P1, Point& P2, Point& P3, float& a, float& b,
        float& c, float& d);
    static bool IntersectionPlaneLine(Point& P1, Point& P2, float a, float b,
        float c, float d, float& u,
        Point& intersection);

    // Vector
    static float AngleBetweenVectors(Point& vec1, Point& vec2);
    static void CrossProduct(Point& u, Point& b, Point& uv);
    static void Normalize(Point& vect);

    // Circle - http://astronomy.swin.edu.au/~pbourke/geometry/circlefrom3/
    static bool IsPerpendicular(Point& pt1, Point& pt2, Point& pt3);
    static bool CalcCircle(Point& pt1, Point& pt2, Point& pt3, Point& centre,
        float& radius);
    static bool CircleEquation(Point& pt1, Point& pt2, Point& pt3, Point& centre,
        float& radius);
};
