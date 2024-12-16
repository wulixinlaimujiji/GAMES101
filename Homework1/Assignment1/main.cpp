#include "Triangle.hpp"
#include "rasterizer.hpp"

#include <iostream>
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

	Eigen::Matrix4f translate;
	translate << 1, 0, 0, -eye_pos[0],
		0, 1, 0, -eye_pos[1],
		0, 0, 1, -eye_pos[2],
		0, 0, 0, 1;

	view = translate * view;

	return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

	// TODO: Implement this function
	// Create the model matrix for rotating the triangle around the Z axis.
	// Then return it.

	// 得到绕z旋转angle角度的矩阵
	Eigen::Matrix4f rotate;
	float angle = rotation_angle * MY_PI / 180.0f;  // 将角度转化为弧度制
	rotate << cos(angle), -sin(angle), 0, 0,
		sin(angle), cos(angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	model = rotate * model;

	return model;
}
// 绕任意过原点的轴旋转
Eigen::Matrix4f get_rotation(Vector3f axis, float angle)
{
	// 获取需要的数据
	Eigen::Matrix3f I = Eigen::Matrix3f::Identity();
	Eigen::Matrix3f N;
	N << 0, -axis(2), axis(1),
		axis(2), 0, -axis(0),
		-axis(1), axis(0), 0;
	float my_angle = angle * MY_PI / 180.0f;
	// 带入罗德里格斯旋转公式
	Eigen::Matrix3f rotate;
	rotate = cos(my_angle) * I + (1 - cos(my_angle)) * axis * axis.transpose() + sin(my_angle) * N;
	// 升维
	Eigen::Matrix4f model;
	model << rotate(0, 0), rotate(0, 1), rotate(0, 2), 0,
		rotate(1, 0), rotate(1, 1), rotate(1, 2), 0,
		rotate(2, 0), rotate(2, 1), rotate(2, 2), 0,
		0, 0, 0, 1;
	return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
	Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

	// TODO: Implement this function
	// Create the projection matrix for the given parameters.
	// Then return it.

	/// 得到正交投影矩阵（先平移再缩放）
	// 获取所需数据
	float fov_Y = eye_fov * MY_PI / 180.0f;
	float yTop = -zNear * tan(fov_Y / 2); // zNear为负值，yTop为正，要加个负号 
	float yBottom = -yTop;
	float xRight = yTop * aspect_ratio;
	float xLeft = -xRight;
	// 平移矩阵
	Eigen::Matrix4f translate;
	translate << 1, 0, 0, -(xLeft + xRight) / 2,
		0, 1, 0, -(yTop + yBottom) / 2,
		0, 0, 1, -(zNear + zFar) / 2,
		0, 0, 0, 1;
	// 缩放矩阵
	Eigen::Matrix4f scale;
	scale << 2 / (xRight - xLeft), 0, 0, 0,
		0, 2 / (yTop - yBottom), 0, 0,
		0, 0, 2 / (zNear - zFar), 0,
		0, 0, 0, 1;
	// 正交投影矩阵
	Eigen::Matrix4f ortho = Eigen::Matrix4f::Identity();
	ortho = scale * translate * ortho;

	/// 得到透视投影转正交投影矩阵
	Eigen::Matrix4f persp_to_ortho;
	persp_to_ortho << zNear, 0, 0, 0,
		0, zNear, 0, 0,
		0, 0, zNear + zFar, -zNear * zFar,
		0, 0, 1, 0;

	///得到透视投影矩阵
	projection = ortho * persp_to_ortho * projection;

	return projection;
}


int main(int argc, const char** argv)
{
	float angle = 0;
	bool command_line = false;
	std::string filename = "output.png";

	if (argc >= 3) {
		command_line = true;
		angle = std::stof(argv[2]); // -r by default
		if (argc == 4) {
			filename = std::string(argv[3]);
		}
	}

	rst::rasterizer r(700, 700);

	Eigen::Vector3f eye_pos = { 0, 0, 5 };

	std::vector<Eigen::Vector3f> pos{ {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };

	std::vector<Eigen::Vector3i> ind{ {0, 1, 2} };

	auto pos_id = r.load_positions(pos);
	auto ind_id = r.load_indices(ind);

	int key = 0;
	int frame_count = 0;

	// 输入旋转所绕的轴
	Eigen::Vector3f axis;
	float x, y, z;
	std::cin >> x >> y >> z;
	axis << x, y, z;

	if (command_line) {
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		//r.set_model(get_model_matrix(angle));
		r.set_model(get_rotation(axis, angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, -0.1, -50)); // 应在z负半轴

		r.draw(pos_id, ind_id, rst::Primitive::Triangle);
		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);

		cv::imwrite(filename, image);

		return 0;
	}

	while (key != 27) {
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		//r.set_model(get_model_matrix(angle));
		r.set_model(get_rotation(axis, angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, -0.1, -50)); // 应在z负半轴

		r.draw(pos_id, ind_id, rst::Primitive::Triangle);

		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::imshow("image", image);
		key = cv::waitKey(10);

		std::cout << "frame count: " << frame_count++ << '\n';

		if (key == 'a') {
			angle += 10;
		}
		else if (key == 'd') {
			angle -= 10;
		}
	}

	return 0;
}
