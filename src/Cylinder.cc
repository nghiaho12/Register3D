#include <cmath>
#include "Cylinder.h"
#include "Point.h"
#include "Misc.h"

Cylinder::Cylinder() {}

Cylinder::Cylinder(Point& start, Point& end, float radius)
{
    SetControlPoints(start, end);

    m_radius = radius;
}

Cylinder::Cylinder(Point& P1, Point& P2, Point& P3)
{
    SetControlPoints(P1, P2, P3);
}

Cylinder::Cylinder(Point& P1, Point& P2, Point& P3, Point& P4)
{
    SetControlPoints(P1, P2, P3, P4);
}

void Cylinder::SetControlPoints(Point& start, Point& end)
{
    ControlPoints.clear();

    ControlPoints.push_back(start);
    ControlPoints.push_back(end);

    Update();
}

void Cylinder::SetControlPoints(Point& P1, Point& P2, Point& P3)
{
    ControlPoints.clear();

    ControlPoints.push_back(P1);
    ControlPoints.push_back(P2);
    ControlPoints.push_back(P3);

    Update();
}

void Cylinder::SetControlPoints(Point& P1, Point& P2, Point& P3, Point& P4)
{
    ControlPoints.clear();

    ControlPoints.push_back(P1);
    ControlPoints.push_back(P2);
    ControlPoints.push_back(P3);
    ControlPoints.push_back(P4);

    Update();
}

void Cylinder::SetStart(Point& start) { m_start = start; }

void Cylinder::SetEnd(Point& end) { m_end = end; }

void Cylinder::SetRadius(float radius)
{
    m_radius = radius;

    if (ControlPoints.size() == 4) {
        Point& P1 = ControlPoints[0];
        Point& P2 = ControlPoints[1];
        Point& P3 = ControlPoints[2];

        Point center;
        float CurrentRadius;

        CircleEquation(P1, P2, P3, center, CurrentRadius);

        Point Vect1(P1.x - center.x, P1.y - center.y, P1.z - center.z);
        Point Vect2(P2.x - center.x, P2.y - center.y, P2.z - center.z);
        Point Vect3(P3.x - center.x, P3.y - center.y, P3.z - center.z);

        // Normalise
        Vect1.x /= CurrentRadius;
        Vect1.y /= CurrentRadius;
        Vect1.z /= CurrentRadius;

        Vect2.x /= CurrentRadius;
        Vect2.y /= CurrentRadius;
        Vect2.z /= CurrentRadius;

        Vect3.x /= CurrentRadius;
        Vect3.y /= CurrentRadius;
        Vect3.z /= CurrentRadius;

        P1.x = center.x + Vect1.x * m_radius;
        P1.y = center.y + Vect1.y * m_radius;
        P1.z = center.z + Vect1.z * m_radius;

        P2.x = center.x + Vect2.x * m_radius;
        P2.y = center.y + Vect2.y * m_radius;
        P2.z = center.z + Vect2.z * m_radius;

        P3.x = center.x + Vect3.x * m_radius;
        P3.y = center.y + Vect3.y * m_radius;
        P3.z = center.z + Vect3.z * m_radius;
    }
}

std::vector<Point>& Cylinder::GetControlPoints() { return ControlPoints; }

Point& Cylinder::GetStart() { return m_start; }

Point& Cylinder::GetEnd() { return m_end; }

float Cylinder::GetRadius() { return m_radius; }

void Cylinder::Update()
{
    if (ControlPoints.size() == 2) {
        m_start = ControlPoints[0];
        m_end = ControlPoints[1];
    } else if (ControlPoints.size() >= 3) {
        Point& P1 = ControlPoints[0];
        Point& P2 = ControlPoints[1];
        Point& P3 = ControlPoints[2];

        Point center;
        float a, b, c, d;

        CircleEquation(P1, P2, P3, center, m_radius);
        PlaneEquation(P1, P2, P3, a, b, c, d);

        Point P4;

        if (ControlPoints.size() == 3) {
            // Add a height control point
            P4.x = center.x + a;
            P4.y = center.y + b;
            P4.z = center.z + c;

            ControlPoints.push_back(P4);
        } else {
            P4 = ControlPoints[3];
        }

        float length = DistancePointPlane(P4, a, b, c, d);

        Point end1(center.x + a * length, center.y + b * length,
            center.z + c * length);
        Point end2(center.x - a * length, center.y - b * length,
            center.z - c * length);

        // Distance Square
        float dist_to_end1 = ((P4.x - end1.x) * (P4.x - end1.x) + (P4.y - end1.y) * (P4.y - end1.y) + (P4.z - end1.z) * (P4.z - end1.z));
        float dist_to_end2 = ((P4.x - end2.x) * (P4.x - end2.x) + (P4.y - end2.y) * (P4.y - end2.y) + (P4.z - end2.z) * (P4.z - end2.z));

        if (dist_to_end1 < dist_to_end2)
            m_end = end1;
        else
            m_end = end2;

        // Force the height control point to be in the middle
        // ControlPoints[3] = m_end;

        m_start = center;
    }
}
