#include "plane.h"

using namespace std;

Plane::Plane(Point3f P1, Point3f P2, Point3f P3)
{
    a = ((P2.y - P1.y) * (P3.z - P1.z) - (P2.z - P1.z) * (P3.y - P1.y));

    b = ((P2.z - P1.z) * (P3.x - P1.x) - (P2.x - P1.x) * (P3.z - P1.z));

    c = ((P2.x - P1.x) * (P3.y - P1.y) - (P2.y - P1.y) * (P3.x - P1.x));

    d = (0 - (a * P1.x + b * P1.y + c * P1.z));
}

float Plane::distance(Point3f P)
{
    return fabs(a * P.x + b * P.y + c * P.z + d) / sqrt(a * a + b * b + c * c);
}