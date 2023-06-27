#pragma once

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class Model
{
private:
	Mat _model_image;
	vector<Point2f> _reference_points;
	vector<Point2f> _target_points;

	vector<KeyPoint> _keypoints;
	Mat _descriptors;
	Mat _scene_show_image;

public:
	Model(string model_path);
	void show();
	void match(string scene_path, vector<Point2f> &scene_reference_points, vector<Point2f> &scene_target_points);
	void show_result(vector<Point2f> scene_target_points, vector<float> target_points_distance);

private:
	void read_model_xml(string xml_path, vector<Point2f>& reference_points, vector<Point2f>& target_points);
};