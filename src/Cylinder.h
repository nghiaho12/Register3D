#pragma once

/*
Cylinder class for drawing.
*/

#include "Point.h"
#include <vector>

class Cylinder {
public:
    Cylinder();
    Cylinder(Point& start, Point& end, float radius); // 2 points
    Cylinder(Point& P1, Point& P2,
        Point& P3); // 3 points for circle, 4th created automatically
    Cylinder(Point& P1, Point& P2, Point& P3,
        Point& P4); // 3 points for circle, 4th for height

    void SetControlPoints(Point& start, Point& end);
    void SetControlPoints(Point& P1, Point& P2, Point& P3);
    void SetControlPoints(Point& P1, Point& P2, Point& P3, Point& P4);

    void SetStart(Point& start);
    void SetEnd(Point& end);
    void SetRadius(float radius);

    std::vector<Point>& GetControlPoints();
    Point& GetStart();
    Point& GetEnd();
    float GetRadius();

    void Update(); // Used by the 4 control point version

private:
    std::vector<Point> ControlPoints;
    // Used for fitting and drawing
    Point m_start, m_end;
    float m_radius;
};

