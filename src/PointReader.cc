#include "PointReader.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool LoadPLYPoints(const std::string& file, std::vector<Point>& points)
{
    std::ifstream input(file);

    if (!input) {
        std::cerr << "can't read: " << file << "\n";
        return false;
    }

    char line[1024];

    // header
    input.getline(line, sizeof(line));

    if (std::string(line) != "ply") {
        std::cerr << "missing ply header" << "\n";
        return false;
    }

    bool found_element_vertex = false;
    int x_offset = -1;
    int y_offset = -1;
    int z_offset = -1;
    int red_offset = -1;
    int green_offset = -1;
    int blue_offset = -1;
    int num_vertex = -1;
    int property_bytes = 0;

    bool binary_ply = false;
    bool float_color = false;

    while (true) {
        input.getline(line, sizeof(line));

        if (input.eof()) {
            break;
        }

        std::string s(line);

        if (s == "format binary_little_endian 1.0") {
            binary_ply = true;
        } else if (s.find("element vertex") != std::string::npos) {
            std::stringstream ss(s);
            std::string not_used;

            ss >> not_used;
            ss >> not_used;
            ss >> num_vertex;
        } else if (s.find("property") != std::string::npos) {
            std::stringstream ss(s);

            std::string not_used;
            std::string type;
            std::string name;

            ss >> not_used;
            ss >> type;
            ss >> name;

            if (name == "x") {
                x_offset = property_bytes;
                if (type != "float") {
                    std::cerr << "expect property x to be type float, not " << type << "\n";
                    return false;
                }
            } else if (name == "y") {
                y_offset = property_bytes;
                if (type != "float") {
                    std::cerr << "expect property y to be type float, not " << type << "\n";
                    return false;
                }
            } else if (name == "z") {
                z_offset = property_bytes;
                if (type != "float") {
                    std::cerr << "expect property z to be type float, not " << type << "\n";
                    return false;
                }
            } else if (name == "red") {
                red_offset = property_bytes;
                if (type == "float") {
                    float_color = true;
                }
            } else if (name == "green") {
                green_offset = property_bytes;
                if (type == "float") {
                    float_color = true;
                }
            } else if (name == "blue") {
                blue_offset = property_bytes;
                if (type == "float") {
                    float_color = true;
                }
            }

            if (type == "float" || type == "int") {
                property_bytes += 4;
            } else if (type == "uchar") {
                property_bytes += 1;
            } else {
                std::cerr << "unsupported property type in PLY file: " << type << "\n";
                return false;
            }
        }

        if (std::string(line) == "end_header") {
            break;
        }
    }

    if (num_vertex == -1) {
        std::cerr << "missing element vertex\n";
        return false;
    }

    if (x_offset == -1) {
        std::cerr << "missing property x\n";
        return false;
    }

    if (y_offset == -1) {
        std::cerr << "missing property y\n";
        return false;
    }

    if (z_offset == -1) {
        std::cerr << "missing property z\n";
        return false;
    }

    if (red_offset == -1) {
        std::cerr << "missing property red\n";
        return false;
    }

    if (green_offset == -1) {
        std::cerr << "missing property green\n";
        return false;
    }

    if (blue_offset == -1) {
        std::cerr << "missing property blue\n";
        return false;
    }

    points.resize(num_vertex);

    std::vector<char> bytes(property_bytes);

    for (int i=0; i < num_vertex; i++) {
        auto &p = points[i];

        if (binary_ply) {
            input.read(bytes.data(), property_bytes);

            float *x = reinterpret_cast<float*>(&bytes[x_offset]);
            float *y = reinterpret_cast<float*>(&bytes[y_offset]);
            float *z = reinterpret_cast<float*>(&bytes[z_offset]);

            p.x = *x;
            p.y = *y;
            p.z = *z;

            if (float_color) {
                float *r = reinterpret_cast<float*>(&bytes[red_offset]);
                float *g = reinterpret_cast<float*>(&bytes[green_offset]);
                float *b = reinterpret_cast<float*>(&bytes[blue_offset]);

                p.r = static_cast<uint8_t>(*r / 255.0);
                p.g = static_cast<uint8_t>(*g / 255.0);
                p.b = static_cast<uint8_t>(*b / 255.0);
            } else {
                uint8_t *r = reinterpret_cast<uint8_t*>(&bytes[red_offset]);
                uint8_t *g = reinterpret_cast<uint8_t*>(&bytes[green_offset]);
                uint8_t *b = reinterpret_cast<uint8_t*>(&bytes[blue_offset]);

                p.r = *r;
                p.g = *g;
                p.b = *b;
            }
        } else {
            std::cerr << "TODO: ascii PLY!\n";
            return false;
        }
    }

    std::cout << file << ", " << num_vertex << " points\n";

    return true;
}
