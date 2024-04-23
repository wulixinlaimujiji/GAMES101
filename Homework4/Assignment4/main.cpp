#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

#define Height 700
#define Width 700
#define MaxSize 4

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < MaxSize) 
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
        << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    // TODO: Implement de Casteljau's algorithm

    // 只有一个点则返回
    if (control_points.size() == 1) {
        return control_points[0];
    }

    // 获取新控制点序列
    std::vector<cv::Point2f> new_points;
    for (int i = 0; i < control_points.size() - 1; ++i) {
        new_points.emplace_back(control_points[i] * (1 - t) + control_points[i + 1] * t);
    }

    // 开始下一轮寻找
    return recursive_bezier(new_points, t);
}

static int antialiasing(cv::Point2f point, cv::Point2f near_pixel_center) {
    // 计算点到相邻像素中心的距离平方
    float x_distance = point.x - near_pixel_center.x;
    float y_distance = point.y - near_pixel_center.y;
    float distance_square = x_distance * x_distance + y_distance * y_distance;
    // 计算相邻像素中心的颜色
    if (distance_square > 1) {
        return 0;
    }
    else {
        float k = -255 * 4 / 3;
        float b = -k;
        return k * distance_square + b;
    }
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.

    for (float t = 0; t <= 1.0f; t += 0.001f) {
        cv::Point2f point = recursive_bezier(control_points, t);
        window.at<cv::Vec3b>(point.y, point.x)[1] = 255;
        
        // 抗锯齿
        int x = (int)point.x, y = (int)point.y;
        cv::Point2f near_pixel_center = { 0, 0 };
        // down
        if (y != 0) {
            near_pixel_center = { x + 0.5f, y - 0.5f };
            int color = antialiasing(point, near_pixel_center);
            if (window.at<cv::Vec3b>(near_pixel_center.y, near_pixel_center.x)[1] < color) {
                window.at<cv::Vec3b>(near_pixel_center.y, near_pixel_center.x)[1] = color;
            }
        }
        // up
        if (y != Height - 1) {
            near_pixel_center = { x + 0.5f, y + 1.5f };
            int color = antialiasing(point, near_pixel_center);
            if (window.at<cv::Vec3b>(near_pixel_center.y, near_pixel_center.x)[1] < color) {
                window.at<cv::Vec3b>(near_pixel_center.y, near_pixel_center.x)[1] = color;
            }
        }
        // left
        if (x != 0) {
            near_pixel_center = { x - 0.5f, y + 0.5f };
            int color = antialiasing(point, near_pixel_center);
            if (window.at<cv::Vec3b>(near_pixel_center.y, near_pixel_center.x)[1] < color) {
                window.at<cv::Vec3b>(near_pixel_center.y, near_pixel_center.x)[1] = color;
            }
        }
        // right
        if (x != Width - 1) {
            near_pixel_center = { x + 1.5f, y + 0.5f };
            int color = antialiasing(point, near_pixel_center);
            if (window.at<cv::Vec3b>(near_pixel_center.y, near_pixel_center.x)[1] < color) {
                window.at<cv::Vec3b>(near_pixel_center.y, near_pixel_center.x)[1] = color;
            }
        }
    }
}

int main() 
{
    cv::Mat window = cv::Mat(Height, Width, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == MaxSize) 
        {
            naive_bezier(control_points, window);
            bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

return 0;
}
