#pragma once
#include <cstdint>

class Point {
public:
    float x, y, z;
    uint8_t r, g, b;

    Point();
    Point(float a, float b, float c);
};
