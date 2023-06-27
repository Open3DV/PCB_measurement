#pragma once
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class Camera
{
private:
    Mat _camera_mtx;
    Mat _distort;
    Mat _depth_map;
public:
    Camera(string param_path);
    void print_params();
    void read_depth(string depth_path);
    void depth2point(float x, float y, float &X, float& Y, float& Z);
    void depth2cloud(vector<Point2f> depth_2d_point, vector<Point3f>& point_3d);
    float get_z(float x, float y);
};
