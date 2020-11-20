#pragma once

#include <vector>
#include <string>
#include "Point.h"

constexpr int MAX_POINTS_ON_GPU = 1000000; // Downsample the points when the point cloud is manipulated

struct RGB {
    unsigned char r, g, b;
};

struct PointCloudData {
    std::string filename;
    std::vector<Point> point;
    std::vector<size_t> table; // Allows a reverseable version of random_shuffle()
    std::vector<RGB> false_colour; // False colour palette
};

