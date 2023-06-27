#include "model.h"
#include "opencv2/opencv.hpp"
#include "opencv2/features2d.hpp"

void Model::read_model_xml(string xml_path, vector<Point2f>& reference_points, vector<Point2f>& target_points)
{
	FileStorage fs(xml_path, FileStorage::READ); //填入读操作
	FileNode rootnode = fs.root();
	FileNodeIterator it;
	for (it = rootnode.begin(); it < rootnode.end(); it++)
	{
		string list_name = (*it).name();

		cout << list_name << endl;
		vector<Point2f>* point_list = 0;
		if (list_name == "reference_points")
		{
			point_list = &reference_points;
		}
		else if (list_name == "target_points")
		{
			point_list = &target_points;
		}
		else
		{
			assert(0);
		}

		FileNodeIterator it2;
		for (it2 = (*it).begin(); it2 < (*it).end(); it2++)
		{
			cout << (*it2).name() << endl;
			int x, y;
			(*it2)["x"] >> x;
			(*it2)["y"] >> y;
			Point2f P = Point2f(x, y);
			point_list->push_back(P);
		}
	}
}

Model::Model(string model_path)
{
	cout << model_path << endl;
	string model_bmp_path = model_path + "/model.bmp";
	string model_xml_path = model_path + "/model.xml";

	read_model_xml(model_xml_path, _reference_points, _target_points);

	_model_image = imread(model_bmp_path, 0);

	int minHessian = 400;
	Ptr<SIFT> detector = SIFT::create(minHessian);
	detector->detectAndCompute(_model_image, noArray(), _keypoints, _descriptors);
	

}

void Model::show()
{
	Mat model_show_image;
	cvtColor(_model_image, model_show_image, COLOR_GRAY2RGB);
	for (int i = 0; i < _reference_points.size(); i++)
	{
		Point P = _reference_points[i];
		circle(model_show_image, P, 4, Scalar(255,0,255), 2);
	}

	for (int i = 0; i < _target_points.size(); i++)
	{
		Point P = _target_points[i];
		circle(model_show_image, P, 4, Scalar(0,255,0), 2);
	}

	imshow("model", model_show_image);
	waitKey(0);
}

void Model::match(string scene_path, vector<Point2f> &scene_reference_points, vector<Point2f> &scene_target_points)
{
	string scene_bmp_path = scene_path + "/bright.bmp";
	Mat scene_image = imread(scene_bmp_path, 0);
	resize(scene_image, scene_image, scene_image.size() / 2);

	std::vector<KeyPoint> scene_keypoints;
	Mat scene_descriptors;

	int minHessian = 400;
	Ptr<SIFT> detector = SIFT::create(minHessian);
	detector->detectAndCompute(scene_image, noArray(), scene_keypoints, scene_descriptors);

	//matching
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<DMatch> > knn_matches;
	matcher->knnMatch(_descriptors, scene_descriptors, knn_matches, 2);

	//-- Filter matches using the Lowe's ratio test
	const float ratio_thresh = 0.7f;
	std::vector<DMatch> good_matches;
	for (size_t i = 0; i < knn_matches.size(); i++)
	{
		if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
		{
			good_matches.push_back(knn_matches[i][0]);
		}
	}

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (size_t i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(_keypoints[good_matches[i].queryIdx].pt);
		scene.push_back(scene_keypoints[good_matches[i].trainIdx].pt);
	}

	Mat H = findHomography(obj, scene, RANSAC);
	//cout << "H size:" << H.cols << endl;
	if (H.cols == 0)return;

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point2f(0, 0);
	obj_corners[1] = Point2f((float)_model_image.cols, 0);
	obj_corners[2] = Point2f((float)_model_image.cols, (float)_model_image.rows);
	obj_corners[3] = Point2f(0, (float)_model_image.rows);
	std::vector<Point2f> scene_corners(4);

	perspectiveTransform(obj_corners, scene_corners, H);

	perspectiveTransform(_reference_points, scene_reference_points, H);
	perspectiveTransform(_target_points, scene_target_points, H);


	//-- Draw lines between the corners (the mapped object in the scene - image_2 )

	cvtColor(scene_image, _scene_show_image, COLOR_GRAY2RGB);
	line(_scene_show_image, scene_corners[0], scene_corners[1], Scalar(0, 255, 0), 1);
	line(_scene_show_image, scene_corners[1], scene_corners[2], Scalar(0, 255, 0), 1);
	line(_scene_show_image, scene_corners[2], scene_corners[3], Scalar(0, 255, 0), 1);
	line(_scene_show_image, scene_corners[3], scene_corners[0], Scalar(0, 255, 0), 1);

	for (int i = 0; i < scene_reference_points.size(); i++)
	{
		circle(_scene_show_image, scene_reference_points[i], 4, Scalar(255, 0, 255), 2);
	}

	for (int i = 0; i < scene_target_points.size(); i++)
	{
		circle(_scene_show_image, scene_target_points[i], 4, Scalar(0, 255, 0), 2);
	}


	//-- Show detected matches
	namedWindow("result", 0);
	imshow("result", _scene_show_image);
	waitKey(1);

	//最后每个点坐标x2，因为以开始图像缩小了一半
	for (int i = 0; i < scene_reference_points.size(); i++)
	{
		scene_reference_points[i] *= 2;
	}

	for (int i = 0; i < scene_target_points.size(); i++)
	{
		scene_target_points[i] *= 2;
	}
}

void Model::show_result(vector<Point2f> scene_target_points, vector<float> target_points_distance)
{
	for (int i = 0; i < scene_target_points.size(); i++)
	{
		putText(_scene_show_image, " "+std::to_string(target_points_distance[i]), scene_target_points[i] / 2, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 255, 0), 2);
	}
	imshow("result", _scene_show_image);
	
}