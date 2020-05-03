#pragma once

// Miscellaneous functions that don't belong anywhere else

#include "ICPPoint.h"
#include "Point.h"
#include <vector>
#include <cmath>
#include <stdexcept>

int MyRandRange(int x); // Range from [0, x-1], used with random_shuffle()

template <class T>
void reverseable_shuffle_forward(std::vector<T>& p, const std::vector<size_t>& table)
{
    if (p.size() != table.size()) {
        throw std::runtime_error("reverseable_shuffle: size mismatch");
    }

    std::vector<T> tmp;

    for (size_t i = 0; i < table.size(); i++) {
        size_t pos = table[i];

        std::swap(p[i], p[pos]);
    }
}

template <class T>
void reverseable_shuffle_backward(std::vector<T>& p, const std::vector<size_t>& table)
{
    if (p.size() != table.size()) {
        throw std::runtime_error("reverseable_shuffle: size mismatch");
    }

    std::vector<T> tmp;

    for (size_t i = 0; i < table.size(); i++) {
        size_t pos = table[table.size() - i - 1];

        std::swap(p[table.size() - 1 - i], p[pos]);
    }
}

inline float Magnitude(const Point& P)
{
    return std::sqrt(P.x * P.x + P.y * P.y + P.z * P.z);
}

inline float Magnitude(const Point& Point1, const Point& Point2)
{
    Point ret;

    ret.x = Point2.x - Point1.x;
    ret.y = Point2.y - Point1.y;
    ret.z = Point2.z - Point1.z;

    return std::sqrt(ret.x * ret.x + ret.y * ret.y + ret.z * ret.z);
}
inline void CrossProduct(const Point& u, const Point& v, Point& uv)
{
    uv.x = u.y * v.z - u.z * v.y;
    uv.y = u.z * v.x - u.x * v.z;
    uv.z = u.x * v.y - u.y * v.x;
}

inline void Normalize(Point& vect)
{
    float mag = std::sqrt(vect.x * vect.x + vect.y * vect.y + vect.z * vect.z);

    vect.x /= mag;
    vect.y /= mag;
    vect.z /= mag;
}

inline float DistancePointPlane(const Point& P, float a, float b, float c, float d)
{
    return std::abs(a * P.x + b * P.y + c * P.z + d) / std::sqrt(a * a + b * b + c * c);
}

inline float AngleBetweenVectors(const Point& vec1, const Point& vec2)
{
    float mag1 = std::sqrt(vec1.x * vec1.x + vec1.y * vec1.y + vec1.z * vec1.z);
    float mag2 = std::sqrt(vec2.x * vec2.x + vec2.y * vec2.y + vec2.z * vec2.z);

    return std::acos((vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z) / (mag1 * mag2));
}

bool PlaneEquation(const Point& P1, const Point& P2, const Point& P3, float& a, float& b,
    float& c, float& d);

bool CircleEquation(const Point& pt1, const Point& pt2, const Point& pt3, Point& centre,
    float& radius);