#include "PointOP.h"
#include <algorithm>
#include <cmath>

Eigen::Matrix4d PointOP::GetTransform(const std::vector<Point> &regpoint1, const std::vector<Point> &regpoint2)
{
    // Get optimal tranformation of regpoint1 -> regpoint2

    if (regpoint1.size() < 3) {
        throw std::runtime_error("regpoint1 must be at least 3");
    }

    if (regpoint2.size() < 3) {
        throw std::runtime_error("regpoint2 must be at least 3");
    }

    if (regpoint1.size() != regpoint2.size()) {
        throw std::runtime_error("regpoint1.size() != regpoint2.size()");
    }

    // Bring all the points to the centroid
    float centroid1[3] = { 0, 0, 0 }, centroid2[3] = { 0, 0, 0 };

    for (size_t i = 0; i < regpoint1.size(); i++) {
        centroid1[0] += regpoint1[i].x;
        centroid1[1] += regpoint1[i].y;
        centroid1[2] += regpoint1[i].z;

        centroid2[0] += regpoint2[i].x;
        centroid2[1] += regpoint2[i].y;
        centroid2[2] += regpoint2[i].z;
    }

    centroid1[0] /= regpoint1.size();
    centroid1[1] /= regpoint1.size();
    centroid1[2] /= regpoint1.size();

    centroid2[0] /= regpoint1.size();
    centroid2[1] /= regpoint1.size();
    centroid2[2] /= regpoint1.size();

    // Use SVD to figure out the rotation matrix
    Eigen::Matrix3d H;
    Eigen::Vector3d A, B;
    H.Zero();

    for (size_t i = 0; i < regpoint1.size(); i++) {
        A(0) = (regpoint1[i].x - centroid1[0]);
        A(1) = (regpoint1[i].y - centroid1[1]);
        A(2) = (regpoint1[i].z - centroid1[2]);

        B(0) = (regpoint2[i].x - centroid2[0]);
        B(1) = (regpoint2[i].y - centroid2[1]);
        B(2) = (regpoint2[i].z - centroid2[2]);

        H += A * B.transpose();
    }

    Eigen::JacobiSVD<Eigen::Matrix3d> svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);

    // Optimal rotation
    Eigen::Matrix3d R = svd.matrixV() * svd.matrixU().transpose();

    Eigen::Matrix4d transform;
    transform.setIdentity();

    Eigen::Matrix4d translate, to_origin, rotation;

    translate.setIdentity();
    to_origin.setIdentity();
    rotation.setIdentity();

    rotation.block(0, 0, 3, 3) = R;

    to_origin(0, 3) = -centroid1[0];
    to_origin(1, 3) = -centroid1[1];
    to_origin(2, 3) = -centroid1[2];

    translate(0, 3) = centroid2[0];
    translate(1, 3) = centroid2[1];
    translate(2, 3) = centroid2[2];

    transform = translate * (rotation * to_origin);

    return transform;
}
