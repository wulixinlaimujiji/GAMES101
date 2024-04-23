//
// Created by LEI XU on 4/27/19.
//

#include "Texture.hpp"

static Eigen::Vector3f getColorlinear(float distance, Eigen::Vector3f color1, Eigen::Vector3f color2) {
    return (1 - distance) * color1 + distance * color2;
}

Eigen::Vector3f Texture::getColorBilinear(float u, float v) {
    Eigen::Vector2f u00 = { (int)(u * width), (int)(v * height) };
    Eigen::Vector2f u01 = u00;
    Eigen::Vector2f u10 = u00;
    Eigen::Vector2f u11 = u00;
    
    if (u00[0] + 1 <  width) {
        u10[0] = u00[0] + 1;
        u11[0] = u00[0] + 1;
    } 
    if (u00[1] + 1 <  height) {
        u01[1] = u00[1] + 1;
        u11[1] = u00[1] + 1;
    } 

    float s = u - u00[0] / width;
    Eigen::Vector3f u0_color = getColorlinear(s, getColor(u00[0] / width, u00[1] / height), getColor(u01[0] / width, u01[1] / height));
    Eigen::Vector3f u1_color = getColorlinear(s, getColor(u10[0] / width, u10[1] / height), getColor(u11[0] / width, u11[1] / height));
    
    float t = v - u00[1] / height;
    Eigen::Vector3f result_color = getColorlinear(t, u0_color, u1_color);
    
    return result_color;
}
