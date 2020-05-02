#pragma once

#include "Point.h"
#include <vector>
#include <wx/wx.h>

constexpr int MAX_POINTS_ON_GPU = 1000000; // Downsample the points when the point cloud is manipulated

const wxString REGISTER3D_VER = wxT("1.1.1");

struct RGB {
    unsigned char r, g, b;
};

struct SharedData {
    std::string filename[2];
    std::vector<Point> point[2];
    std::vector<size_t> table[2]; // Allows a reverseable version of random_shuffle()
    std::vector<RGB> false_colour[2]; // False colour palette

    std::vector<uint8_t> false_colour_r, false_colour_g, false_colour_b;
    float false_colour_min_z, false_colour_max_z;
};

