#include "Misc.h"
#include <random> // For better random number, Windows one sucks big time

std::mt19937 eng;

int MyRandRange(int x)
{
    std::uniform_int_distribution<> unif(0, x - 1);

    return unif(eng);
}

bool PlaneEquation(const Point& P1, const Point& P2, const Point& P3, float& a, float& b,
    float& c, float& d)
{
    // First point
    float x1 = P1.x;
    float y1 = P1.y;
    float z1 = P1.z;

    // Second point
    float x2 = P2.x;
    float y2 = P2.y;
    float z2 = P2.z;

    // Third point
    float x3 = P3.x;
    float y3 = P3.y;
    float z3 = P3.z;

    // Find co-effecients of Plane equation
    a = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
    b = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
    c = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
    d = -(x1 * (y2 * z3 - y3 * z2) + x2 * (y3 * z1 - y1 * z3) + x3 * (y1 * z2 - y2 * z1));

    // Degenerate plane
    if (a == 0 && b == 0 && c == 0) {
        return false;
    }

    // Normalise
    float mag = sqrt(a * a + b * b + c * c);

    a /= mag;
    b /= mag;
    c /= mag;
    d /= mag;

    return true;
}

bool IsPerpendicular(const Point& pt1, const Point& pt2, const Point& pt3)
{
    constexpr double TOL = 1e-6;

    double yDelta_a = pt2.y - pt1.y;
    double xDelta_a = pt2.x - pt1.x;
    double yDelta_b = pt3.y - pt2.y;
    double xDelta_b = pt3.x - pt2.x;

    // checking whether the line of the two pts are vertical
    if (std::abs(xDelta_a) < TOL && std::abs(yDelta_b) < TOL) {
        return false;
    }

    if (std::abs(yDelta_a) < TOL) {
        return true;
    } else if (std::abs(yDelta_b) < TOL) {
        return true;
    } else if (std::abs(xDelta_a) < TOL) {
        return true;
    } else if (std::abs(xDelta_b) < TOL) {
        return true;
    } else {
        return false;
    }
}

bool CalcCircle(const Point& pt1, const Point& pt2, const Point& pt3, Point& centre,
    float& radius)
{
    constexpr double TOL = 1e-6;

    double yDelta_a = pt2.y - pt1.y;
    double xDelta_a = pt2.x - pt1.x;
    double yDelta_b = pt3.y - pt2.y;
    double xDelta_b = pt3.x - pt2.x;

    if (std::abs(xDelta_a) < TOL && std::abs(yDelta_b) < TOL) {
        centre.x = 0.5 * (pt2.x + pt3.x);
        centre.y = 0.5 * (pt1.y + pt2.y);
        centre.z = pt1.z;
        radius = Magnitude(centre, pt1); // calc. radius

        return true;
    }

    // IsPerpendicular() assure that xDelta(s) are not zero
    double aSlope = yDelta_a / xDelta_a;
    double bSlope = yDelta_b / xDelta_b;

    if (std::abs(aSlope - bSlope) < TOL) // checking whether the given points are colinear.
        return false;

    // calc center
    centre.x = (aSlope * bSlope * (pt1.y - pt3.y) + bSlope * (pt1.x + pt2.x) - aSlope * (pt2.x + pt3.x)) / (2 * (bSlope - aSlope));

    centre.y = -1 * (centre.x - (pt1.x + pt2.x) / 2) / aSlope + (pt1.y + pt2.y) / 2;

    centre.z = pt1.z;

    radius = Magnitude(centre, pt1);

    return true;
}

bool CircleEquation(const Point& pt1, const Point& pt2, const Point& pt3, Point& centre,
    float& radius)
{
    if (!IsPerpendicular(pt1, pt2, pt3))
        return CalcCircle(pt1, pt2, pt3, centre, radius);
    else if (!IsPerpendicular(pt1, pt3, pt2))
        return CalcCircle(pt1, pt3, pt2, centre, radius);
    else if (!IsPerpendicular(pt2, pt1, pt3))
        return CalcCircle(pt2, pt1, pt3, centre, radius);
    else if (!IsPerpendicular(pt2, pt3, pt1))
        return CalcCircle(pt2, pt3, pt1, centre, radius);
    else if (!IsPerpendicular(pt3, pt2, pt1))
        return CalcCircle(pt3, pt2, pt1, centre, radius);
    else if (!IsPerpendicular(pt3, pt1, pt2))
        return CalcCircle(pt3, pt1, pt2, centre, radius);
    else {
        return false;
    }

    return true;
}