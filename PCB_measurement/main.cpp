#include "opencv2/opencv.hpp"
#include "model.h"
#include "camera.h"
#include "plane.h"

int main(int argc, char** argv)
{
	string model_path = "../../model";
	string scene_root = "../../PCBs";
	string cam_param = "../../PCBs/param.txt";

	Camera cam(cam_param);
	cam.print_params();
	

	Model model(model_path);
	model.show();

	for (int i = 1; i <= 5; i++)
	{
		string scene_path = scene_root + "/" + std::to_string(i);
		
		vector<Point2f> scene_reference_points;
		vector<Point2f> scene_target_points;

		cam.read_depth(scene_path + "/depth.tiff");

		model.match(scene_path, scene_reference_points, scene_target_points);

		//cout << scene_reference_points << endl;
		//cout << scene_target_points << endl;

		vector<Point3f> scene_reference_points_3d;
		vector<Point3f> scene_target_points_3d;
		cam.depth2cloud(scene_reference_points, scene_reference_points_3d);
		cam.depth2cloud(scene_target_points, scene_target_points_3d);

		//cout << scene_reference_points_3d << endl;
		//cout << scene_target_points_3d << endl;

		Plane plane(scene_reference_points_3d[0], scene_reference_points_3d[1], scene_reference_points_3d[2]);
		vector<float> target_points_distance;
		for (int j = 0; j < scene_target_points_3d.size(); j++)
		{
			float distance = plane.distance(scene_target_points_3d[j]);
			cout << j << ":" << distance <<"  ";
			target_points_distance.push_back(distance);
		}
		cout << endl;

		model.show_result(scene_target_points, target_points_distance);
		waitKey(0);
	}

	return 0;
}