#include "Math2.h"
#include "Matrix.h"
#include <math.h>

void Math2::ApplyRotation(Point& P, double angle_x, double angle_y,
    double angle_z)
{
    double x = P.x;
    double y = P.y;
    double z = P.z;

    double new_x, new_y, new_z;

    // Rotate about X
    new_x = x;
    new_y = cos(angle_x) * y - sin(angle_x) * z;
    new_z = sin(angle_x) * y + cos(angle_x) * z;

    x = new_x;
    y = new_y;
    z = new_z;

    // Rotate about Y
    new_x = cos(angle_y) * x + sin(angle_y) * z;
    new_y = y;
    new_z = -sin(angle_y) * x + cos(angle_y) * z;

    P.x = new_x;
    P.y = new_y;
    P.z = new_z;

    x = new_x;
    y = new_y;
    z = new_z;

    // Rotate about Z
    new_x = cos(angle_z) * x - sin(angle_z) * y;
    new_y = sin(angle_z) * x + cos(angle_z) * y;
    new_z = z;

    P.x = new_x;
    P.y = new_y;
    P.z = new_z;
}

void Math2::ApplyRotation(vector<Point>& points, double angle_x, double angle_y,
    double angle_z)
{
    for (unsigned int i = 0; i < points.size(); i++) {
        double x = points[i].x;
        double y = points[i].y;
        double z = points[i].z;

        double new_x, new_y, new_z;

        // Rotate about X
        new_x = x;
        new_y = cos(angle_x) * y - sin(angle_x) * z;
        new_z = sin(angle_x) * y + cos(angle_x) * z;

        x = new_x;
        y = new_y;
        z = new_z;

        // Rotate about Y - Fix
        new_x = cos(angle_y) * x + sin(angle_y) * z;
        new_y = y;
        new_z = -sin(angle_y) * x + cos(angle_y) * z;

        points[i].x = new_x;
        points[i].y = new_y;
        points[i].z = new_z;

        x = new_x;
        y = new_y;
        z = new_z;

        // Rotate about Z
        new_x = cos(angle_z) * x - sin(angle_z) * y;
        new_y = sin(angle_z) * x + cos(angle_z) * y;
        new_z = z;

        points[i].x = new_x;
        points[i].y = new_y;
        points[i].z = new_z;
    }
}

void Math2::RotatePoint(Point& P, float angle, float x, float y, float z)
{
    float mag_sq = x * x + y * y + z * z;

    if (mag_sq != 1.0) {
        float mag = sqrt(mag_sq);

        x /= mag;
        y /= mag;
        z /= mag;
    }

    float cos_angle = cos(angle);
    float sin_angle = sin(angle);

    // Create a rotation matrix
    // [ a b c 0 ]
    // [ d e f 0 ]
    // [ g h i 0 ]
    // [ 0 0 0 1]

    float a = x * x * (1.0f - cos_angle) + cos_angle;
    float b = x * y * (1.0f - cos_angle) - z * sin_angle;
    float c = x * z * (1.0f - cos_angle) + y * sin_angle;
    float d = y * x * (1.0f - cos_angle) + z * sin_angle;
    float e = y * y * (1.0f - cos_angle) + cos_angle;
    float f = y * z * (1.0f - cos_angle) - x * sin_angle;
    float g = x * z * (1.0f - cos_angle) - y * sin_angle;
    float h = y * z * (1.0f - cos_angle) + x * sin_angle;
    float i = z * z * (1.0f - cos_angle) + cos_angle;

    float new_x = a * P.x + b * P.y + c * P.z;
    float new_y = d * P.x + e * P.y + f * P.z;
    float new_z = g * P.x + h * P.y + i * P.z;

    P.x = new_x;
    P.y = new_y;
    P.z = new_z;
}

void Math2::RotatePoints(vector<Point>& points, float angle, float x, float y,
    float z)
{
    float mag_sq = x * x + y * y + z * z;

    if (mag_sq != 1.0) {
        float mag = sqrt(mag_sq);

        x /= mag;
        y /= mag;
        z /= mag;
    }

    float cos_angle = cos(angle);
    float sin_angle = sin(angle);

    // Create a rotation matrix
    // [ a b c 0 ]
    // [ d e f 0 ]
    // [ g h i 0 ]
    // [ 0 0 0 1]

    float a = x * x * (1 - cos_angle) + cos_angle;
    float b = x * y * (1 - cos_angle) - z * sin_angle;
    float c = x * z * (1 - cos_angle) + y * sin_angle;
    float d = y * x * (1 - cos_angle) + z * sin_angle;
    float e = y * y * (1 - cos_angle) + cos_angle;
    float f = y * z * (1 - cos_angle) - x * sin_angle;
    float g = x * z * (1 - cos_angle) - y * sin_angle;
    float h = y * z * (1 - cos_angle) + x * sin_angle;
    float i = z * z * (1 - cos_angle) + cos_angle;

    for (unsigned int z = 0; z < points.size(); z++) {
        Point& P = points[z];

        float NewX = a * P.x + b * P.y + c * P.z;
        float NewY = d * P.x + e * P.y + f * P.z;
        float NewZ = g * P.x + h * P.y + i * P.z;

        P.x = NewX;
        P.y = NewY;
        P.z = NewZ;
    }
}

void Math2::PCA(const vector<Point>& points, Point& PC1, Point& PC2, Point& PC3,
    Point& mean)
{
    mean.x = 0;
    mean.y = 0;
    mean.z = 0;

    for (unsigned int i = 0; i < points.size(); i++) {
        mean.x += points[i].x;
        mean.y += points[i].y;
        mean.z += points[i].z;
    }

    mean.x /= (float)points.size();
    mean.y /= (float)points.size();
    mean.z /= (float)points.size();

    // Shift data to origin
    Matrix M(points.size(), 3);

    for (unsigned int i = 0; i < points.size(); i++) {
        M.Set(i, 0, points[i].x - mean.x);
        M.Set(i, 1, points[i].y - mean.y);
        M.Set(i, 2, points[i].z - mean.z);
    }

    Matrix U(points.size(), 3), S(3, 3), V(3, 3);

    M.SVD(U, S, V);

    PC1.x = V.Get(0, 0);
    PC1.y = V.Get(1, 0);
    PC1.z = V.Get(2, 0);

    PC2.x = V.Get(0, 1);
    PC2.y = V.Get(1, 1);
    PC2.z = V.Get(2, 1);

    PC3.x = V.Get(0, 2);
    PC3.y = V.Get(1, 2);
    PC3.z = V.Get(2, 2);
}

void Math2::PlaneRegression(const vector<Point>& points, double& A, double& B,
    double& C, double& D)
{
    Point PC1, PC2, PC3, mean;

    PCA(points, PC1, PC2, PC3, mean);

    float MeanNormal = mean.x * PC3.x + mean.y * PC3.y + mean.z * PC3.z;

    A = PC3.x;
    B = PC3.y;
    C = PC3.z;
    D = -MeanNormal;
}

float Math2::Magnitude(Point& P)
{
    return sqrt(P.x * P.x + P.y * P.y + P.z * P.z);
}

float Math2::Magnitude(Point& Point1, Point& Point2)
{
    Point Vector;

    Vector.x = Point2.x - Point1.x;
    Vector.y = Point2.y - Point1.y;
    Vector.z = Point2.z - Point1.z;

    return (float)sqrt(Vector.x * Vector.x + Vector.y * Vector.y + Vector.z * Vector.z);
}

float Math2::DistancePointLine(Point& P, Point& line_start, Point& line_end)
{
    float distance, u;
    Point DirVector;

    DistancePointLine(P, line_start, line_end, distance, u, DirVector);

    return distance;
}

void Math2::DistancePointLine(Point& P, Point& line_start, Point& line_end,
    float& distance, float& U, Point& DirVector)
{
    float line_mag;
    Point intersection;

    line_mag = Magnitude(line_end, line_start);

    if (&U != NULL) {
        U = (((P.x - line_start.x) * (line_end.x - line_start.x)) + ((P.y - line_start.y) * (line_end.y - line_start.y)) + ((P.z - line_start.z) * (line_end.z - line_start.z))) / (line_mag * line_mag);

        // if( U < 0.0f || U > 1.0f )
        // return false;   // closest point does not fall within the line segment
    }

    intersection.x = line_start.x + U * (line_end.x - line_start.x);
    intersection.y = line_start.y + U * (line_end.y - line_start.y);
    intersection.z = line_start.z + U * (line_end.z - line_start.z);

    distance = Magnitude(P, intersection);

    if (&DirVector != NULL) {
        DirVector.x = P.x - intersection.x;
        DirVector.y = P.y - intersection.y;
        DirVector.z = P.z - intersection.z;

        float mag = sqrt(DirVector.x * DirVector.x + DirVector.y * DirVector.y + DirVector.z * DirVector.z);

        DirVector.x /= mag;
        DirVector.y /= mag;
        DirVector.z /= mag;
    }
    // return true;
}

bool Math2::PlaneEquation(Point& P1, Point& P2, Point& P3, float& a, float& b,
    float& c, float& d)
{
    // First point
    float x1 = P1.x;
    float y1 = P1.y;
    float z1 = P1.z;

    // Second point
    float x2 = P2.x;
    float y2 = P2.y;
    float z2 = P2.z;

    // Third point
    float x3 = P3.x;
    float y3 = P3.y;
    float z3 = P3.z;

    // Find co-effecients of Plane equation
    a = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
    b = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
    c = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
    d = -(x1 * (y2 * z3 - y3 * z2) + x2 * (y3 * z1 - y1 * z3) + x3 * (y1 * z2 - y2 * z1));

    if ((a == 0 && b == 0 && c == 0)) // Degenerate plane
        return false;

    // Normalise
    float mag = sqrt(a * a + b * b + c * c);

    a /= mag;
    b /= mag;
    c /= mag;
    d /= mag;

    return true;
}

float Math2::DistancePointPlane(Point& P, float a, float b, float c, float d)
{
    return fabs(a * P.x + b * P.y + c * P.z + d) / sqrt(a * a + b * b + c * c);
}

bool Math2::IntersectionPlaneLine(Point& P1, Point& P2, float a, float b,
    float c, float d, float& u,
    Point& intersection)
{
    float num = a * P1.x + b * P1.y + c * P1.z + d;
    float den = a * (P1.x - P2.x) + b * (P1.y - P2.y) + c * (P1.z - P2.z);

    if (den == 0)
        return false;

    u = num / den;

    intersection.x = P1.x + u * (P2.x - P1.x);
    intersection.y = P1.y + u * (P2.y - P1.y);
    intersection.z = P1.z + u * (P2.z - P1.z);

    return true;
}

float Math2::AngleBetweenVectors(Point& vec1, Point& vec2)
{
    float mag1 = sqrt(vec1.x * vec1.x + vec1.y * vec1.y + vec1.z * vec1.z);
    float mag2 = sqrt(vec2.x * vec2.x + vec2.y * vec2.y + vec2.z * vec2.z);

    return acos((vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z) / (mag1 * mag2));
}

bool Math2::IsPerpendicular(Point& pt1, Point& pt2, Point& pt3)
{
    double yDelta_a = pt2.y - pt1.y;
    double xDelta_a = pt2.x - pt1.x;
    double yDelta_b = pt3.y - pt2.y;
    double xDelta_b = pt3.x - pt2.x;

    // checking whether the line of the two pts are vertical
    if (fabs(xDelta_a) <= 0.000000001 && fabs(yDelta_b) <= 0.000000001) {
        return false;
    }

    if (fabs(yDelta_a) <= 0.0000001) {
        return true;
    } else if (fabs(yDelta_b) <= 0.0000001) {
        return true;
    } else if (fabs(xDelta_a) <= 0.000000001) {
        return true;
    } else if (fabs(xDelta_b) <= 0.000000001) {
        return true;
    } else
        return false;
}

bool Math2::CalcCircle(Point& pt1, Point& pt2, Point& pt3, Point& centre,
    float& radius)
{
    double yDelta_a = pt2.y - pt1.y;
    double xDelta_a = pt2.x - pt1.x;
    double yDelta_b = pt3.y - pt2.y;
    double xDelta_b = pt3.x - pt2.x;

    if (fabs(xDelta_a) <= 0.000000001 && fabs(yDelta_b) <= 0.000000001) {
        centre.x = 0.5 * (pt2.x + pt3.x);
        centre.y = 0.5 * (pt1.y + pt2.y);
        centre.z = pt1.z;
        radius = Magnitude(centre, pt1); // calc. radius

        return true;
    }

    // IsPerpendicular() assure that xDelta(s) are not zero
    double aSlope = yDelta_a / xDelta_a;
    double bSlope = yDelta_b / xDelta_b;

    if (fabs(aSlope - bSlope) <= 0.000000001) // checking whether the given points are colinear.
        return false;

    // calc center
    centre.x = (aSlope * bSlope * (pt1.y - pt3.y) + bSlope * (pt1.x + pt2.x) - aSlope * (pt2.x + pt3.x)) / (2 * (bSlope - aSlope));

    centre.y = -1 * (centre.x - (pt1.x + pt2.x) / 2) / aSlope + (pt1.y + pt2.y) / 2;

    centre.z = pt1.z;

    radius = Magnitude(centre, pt1);

    return true;
}

bool Math2::CircleEquation(Point& pt1, Point& pt2, Point& pt3, Point& centre,
    float& radius)
{
    if (!IsPerpendicular(pt1, pt2, pt3))
        return CalcCircle(pt1, pt2, pt3, centre, radius);
    else if (!IsPerpendicular(pt1, pt3, pt2))
        return CalcCircle(pt1, pt3, pt2, centre, radius);
    else if (!IsPerpendicular(pt2, pt1, pt3))
        return CalcCircle(pt2, pt1, pt3, centre, radius);
    else if (!IsPerpendicular(pt2, pt3, pt1))
        return CalcCircle(pt2, pt3, pt1, centre, radius);
    else if (!IsPerpendicular(pt3, pt2, pt1))
        return CalcCircle(pt3, pt2, pt1, centre, radius);
    else if (!IsPerpendicular(pt3, pt1, pt2))
        return CalcCircle(pt3, pt1, pt2, centre, radius);
    else {
        return false;
    }

    return true;
}

void Math2::CrossProduct(Point& u, Point& v, Point& uv)
{
    uv.x = u.y * v.z - u.z * v.y;
    uv.y = u.z * v.x - u.x * v.z;
    uv.z = u.x * v.y - u.y * v.x;
}

void Math2::Normalize(Point& vect)
{
    float mag = sqrt(vect.x * vect.x + vect.y * vect.y + vect.z * vect.z);

    vect.x /= mag;
    vect.y /= mag;
    vect.z /= mag;
}
