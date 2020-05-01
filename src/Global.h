#pragma once

#include "Point.h"
#include <vector>
#include <wx/wx.h>

constexpr int MAX_POINTS_ON_GPU = 1000000; // Downsample the points when the point cloud is manipulated

const wxString REGISTER3D_VER = wxT("1.1.1");

struct RGB {
    unsigned char r, g, b;
};

struct _Global {
    std::vector<Point> scan1, scan2;

    int ID_1, ID_2;
    std::vector<unsigned int> table1, table2; // Allows a reverseable version of random_shuffle()
    wxString pwd;

    // False colour palette
    std::vector<RGB> false_colour1, false_colour2; // for scan1, scan2
    unsigned char *false_colour_r, *false_colour_g, *false_colour_b;
    float false_colour_min_z, false_colour_max_z;
};

