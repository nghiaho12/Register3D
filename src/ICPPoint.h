#ifndef ICPPOINT_H
#define ICPPOINT_H

#include "Point.h"

class ICPPoint : public Point {
public:
    ICPPoint() {}
    ICPPoint(float a, float b, float c)
    {
        x = a;
        y = b;
        z = c;
    }

    ICPPoint(Point& P)
    {
        x = P.x;
        y = P.y;
        z = P.z;
        r = P.r;
        g = P.g;
        b = P.b;
    }

public:
    Point nearest;
    double dist_sq;
};

#endif
