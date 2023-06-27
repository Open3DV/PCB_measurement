#pragma once
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class Plane
{
private:
	//平面方程：a*x + b*y + c*z + d = 0
	float a;
	float b;
	float c;
	float d;
public:
	Plane(Point3f P1, Point3f P2, Point3f P3);
	float distance(Point3f P);
};