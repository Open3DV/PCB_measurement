#include "camera.h"
#include <fstream>

using namespace std;

Camera::Camera(string param_path)
{
	ifstream param_file(param_path);
	_camera_mtx = Mat::zeros(3, 3, CV_32FC1);
	_distort = Mat::zeros(1, 5, CV_32FC1);

	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			param_file >> _camera_mtx.at<float>(y, x);
		}
	}
	for (int i = 0; i < 5; i++)
	{
		param_file >> _distort.at<float>(0, i);
	}
}

void Camera::read_depth(string depth_path)
{
	_depth_map = imread(depth_path, -1);
}

void Camera::print_params()
{
	cout << _camera_mtx << endl;
	cout << _distort << endl;
}

float Camera::get_z(float x, float y)
{
	Mat smoothed_depth;
	medianBlur(_depth_map, smoothed_depth, 3);
	GaussianBlur(smoothed_depth, smoothed_depth, Size(3, 3), 2);
	float z = smoothed_depth.at<float>(y, x);
	return z;
}

void Camera::depth2point(float x, float y, float& X, float& Y, float& Z)
{
	float z = get_z(x, y);
	Mat src = Mat(1,2,CV_32FC1);
	src.at<float>(0, 0) = x;
	src.at<float>(0, 1) = y;
	//cout << "x:" << x << ",y:" << y << endl;

	Mat dst = Mat(1, 2, CV_32FC1);

	undistortPoints(src, dst, _camera_mtx, _distort);
	//cout << "dst:" << dst << endl;

	X = dst.at<float>(0, 0) * z;
	Y = dst.at<float>(0, 1) * z;
	Z = z;

}

void Camera::depth2cloud(vector<Point2f> depth_2d_point, vector<Point3f>& point_3d)
{
	point_3d.clear();
	float X, Y, Z;

	for (int i = 0; i < depth_2d_point.size(); i++)
	{
		depth2point(depth_2d_point[i].x, depth_2d_point[i].y, X, Y, Z);
		Point3f P(X, Y, Z);
		point_3d.push_back(P);
	}
}