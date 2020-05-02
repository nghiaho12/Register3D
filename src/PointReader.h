#pragma once

#include <string>
#include <vector>

#include <Eigen/Dense>

#include "Point.h"

/**
 * @brief
 *
 * @param file PLY file, ascii or binary
 * @param points if not null, PLY loads into points
 * @param output_file if not null, PLY is loaded and saved to output_file with transform applied
 * @param transform used with output_file
 */
bool ReadPLYPoints(
    const std::string& file,
    std::vector<Point>* points,
    const std::string* output_file,
    const Eigen::Matrix4d* transform);
