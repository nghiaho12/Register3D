#include "PointOP.h"
#include "Math2.h"
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

using namespace std;

void PointOP::ApplyTransform(vector<Point>& P, Matrix& T)
{
    for (unsigned int i = 0; i < P.size(); i++) {
        float x = P[i].x;
        float y = P[i].y;
        float z = P[i].z;

        float new_x = x * T.Get(0, 0) + y * T.Get(0, 1) + z * T.Get(0, 2) + T.Get(0, 3);
        float new_y = x * T.Get(1, 0) + y * T.Get(1, 1) + z * T.Get(1, 2) + T.Get(1, 3);
        float new_z = x * T.Get(2, 0) + y * T.Get(2, 1) + z * T.Get(2, 2) + T.Get(2, 3);

        P[i].x = new_x;
        P[i].y = new_y;
        P[i].z = new_z;
    }
}

void PointOP::ApplyTransform(vector<ICPPoint>& P, Matrix& T)
{
    for (unsigned int i = 0; i < P.size(); i++) {
        float x = P[i].x;
        float y = P[i].y;
        float z = P[i].z;

        float new_x = x * T.Get(0, 0) + y * T.Get(0, 1) + z * T.Get(0, 2) + T.Get(0, 3);
        float new_y = x * T.Get(1, 0) + y * T.Get(1, 1) + z * T.Get(1, 2) + T.Get(1, 3);
        float new_z = x * T.Get(2, 0) + y * T.Get(2, 1) + z * T.Get(2, 2) + T.Get(2, 3);

        P[i].x = new_x;
        P[i].y = new_y;
        P[i].z = new_z;
    }
}

void PointOP::GetTransform(vector<Point> regpoint1, vector<Point> regpoint2,
    Matrix& transform)
{
    // Get optimal tranformation of regpoint1 -> regpoint2

    if (regpoint1.size() < 3) {
        fprintf(stderr, "Registration points must be at least  3\n");
        exit(0);
    }

    if (regpoint2.size() < 3) {
        fprintf(stderr, "Registration points must be at least  3\n");
        exit(0);
    }

    if (regpoint1.size() != regpoint2.size()) {
        fprintf(stderr, "regpoint1 and regpoint2 aren't the same size\n");
        exit(0);
    }

    // Bring all the points to the centroid
    float centroid1[3] = { 0, 0, 0 }, centroid2[3] = { 0, 0, 0 };

    for (unsigned int i = 0; i < regpoint1.size(); i++) {
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

    for (unsigned int i = 0; i < regpoint1.size(); i++) {
        regpoint1[i].x -= centroid1[0];
        regpoint1[i].y -= centroid1[1];
        regpoint1[i].z -= centroid1[2];

        regpoint2[i].x -= centroid2[0];
        regpoint2[i].y -= centroid2[1];
        regpoint2[i].z -= centroid2[2];
    }

    // Use SVD to figure out the rotation matrix
    Matrix MA(3, 1), MB(1, 3), MC(3, 3), R(3, 3), H(3, 3);
    Matrix U(3, 3), S(3, 3), V(3, 3); // SVD

    H.Zero();

    for (unsigned int i = 0; i < regpoint1.size(); i++) {
        MA.Set(0, 0, regpoint1[i].x);
        MA.Set(1, 0, regpoint1[i].y);
        MA.Set(2, 0, regpoint1[i].z);

        MB.Set(0, 0, regpoint2[i].x);
        MB.Set(0, 1, regpoint2[i].y);
        MB.Set(0, 2, regpoint2[i].z);

        MC = MA * MB;

        H += MC;
    }

    H.SVD(U, S, V);

    // Optimal rotation
    R = V * U.Transpose();

    transform.LoadIdentity();

    Matrix translate(4, 4), to_origin(4, 4), rotation(4, 4);

    translate.LoadIdentity();
    to_origin.LoadIdentity();
    rotation.LoadIdentity();

    // rotation
    rotation.Set(0, 0, R.Get(0, 0));
    rotation.Set(0, 1, R.Get(0, 1));
    rotation.Set(0, 2, R.Get(0, 2));
    rotation.Set(1, 0, R.Get(1, 0));
    rotation.Set(1, 1, R.Get(1, 1));
    rotation.Set(1, 2, R.Get(1, 2));
    rotation.Set(2, 0, R.Get(2, 0));
    rotation.Set(2, 1, R.Get(2, 1));
    rotation.Set(2, 2, R.Get(2, 2));

    to_origin.Set(0, 3, -centroid1[0]);
    to_origin.Set(1, 3, -centroid1[1]);
    to_origin.Set(2, 3, -centroid1[2]);

    translate.Set(0, 3, centroid2[0]);
    translate.Set(1, 3, centroid2[1]);
    translate.Set(2, 3, centroid2[2]);

    printf("transform\n");

    transform = translate * (rotation * to_origin);

    transform.Print();
}

void PointOP::GetTransform2(vector<Point>& regpoint1, vector<Point>& regpoint2,
    Matrix& transform)
{
    // Get optimal tranformation of regpoint1 -> regpoint2

    if (regpoint1.size() != 3) {
        fprintf(stderr, "Registration points must be size 3\n");
        exit(0);
    }

    if (regpoint2.size() != 3) {
        fprintf(stderr, "Registration points must be size 3\n");
        exit(0);
    }

    // Bring all the points to the centroid
    float centroid1[3], centroid2[3];

    printf("Registration points\n");

    printf("%f %f %f\n", regpoint1[0].x, regpoint1[0].y, regpoint1[0].z);
    printf("%f %f %f\n", regpoint1[1].x, regpoint1[1].y, regpoint1[1].z);
    printf("%f %f %f\n", regpoint1[2].x, regpoint1[2].y, regpoint1[2].z);

    printf("\n");

    printf("%f %f %f\n", regpoint2[0].x, regpoint2[0].y, regpoint2[0].z);
    printf("%f %f %f\n", regpoint2[1].x, regpoint2[1].y, regpoint2[1].z);
    printf("%f %f %f\n", regpoint2[2].x, regpoint2[2].y, regpoint2[2].z);

    // Registration point 1
    centroid1[0] = (regpoint1.at(0).x + regpoint1.at(1).x + regpoint1.at(2).x) / 3.0;
    centroid1[1] = (regpoint1.at(0).y + regpoint1.at(1).y + regpoint1.at(2).y) / 3.0;
    centroid1[2] = (regpoint1.at(0).z + regpoint1.at(1).z + regpoint1.at(2).z) / 3.0;

    // Registration point 2
    centroid2[0] = (regpoint2.at(0).x + regpoint2.at(1).x + regpoint2.at(2).x) / 3.0;
    centroid2[1] = (regpoint2.at(0).y + regpoint2.at(1).y + regpoint2.at(2).y) / 3.0;
    centroid2[2] = (regpoint2.at(0).z + regpoint2.at(1).z + regpoint2.at(2).z) / 3.0;

    printf("Centroid\n");
    printf("%f %f %f\n", centroid1[0], centroid1[1], centroid1[2]);
    printf("%f %f %f\n", centroid2[0], centroid2[1], centroid2[2]);

    for (unsigned int i = 0; i < 3; i++) {
        regpoint1[i].x -= centroid1[0];
        regpoint1[i].y -= centroid1[1];
        regpoint1[i].z -= centroid1[2];

        regpoint2[i].x -= centroid2[0];
        regpoint2[i].y -= centroid2[1];
        regpoint2[i].z -= centroid2[2];
    }

    // Angle between first point
    float angle1 = Math2::AngleBetweenVectors(regpoint1[0], regpoint2[0]);
    float angle2 = Math2::AngleBetweenVectors(regpoint1[1], regpoint2[1]);
    float angle3 = Math2::AngleBetweenVectors(regpoint1[2], regpoint2[2]);

    printf("Angle between vectors\n");
    printf("%f %f %f\n", angle1 * 180 / M_PI, angle2 * 180 / M_PI,
        angle3 * 180 / M_PI);

    // Average rotation axis
    Point cross1, cross2, cross3;

    Math2::CrossProduct(regpoint1[0], regpoint2[0], cross1);
    Math2::CrossProduct(regpoint1[1], regpoint2[1], cross2);
    Math2::CrossProduct(regpoint1[2], regpoint2[2], cross3);

    Math2::Normalize(cross1);
    Math2::Normalize(cross2);
    Math2::Normalize(cross3);

    printf("Cross\n");
    printf("%f %f %f\n", cross1.x, cross1.y, cross1.z);
    printf("%f %f %f\n", cross2.x, cross2.y, cross2.z);
    printf("%f %f %f\n", cross3.x, cross3.y, cross3.z);
}
