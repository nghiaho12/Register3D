#pragma once

#include <string>
#include <vector>
#include "Point.h"

bool LoadPLYPoints(const std::string& file, std::vector<Point>& points);
