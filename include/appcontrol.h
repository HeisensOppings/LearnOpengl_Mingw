#ifndef __CONTEXTCTRL_H__
#define __CONTEXTCTRL_H__

#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

class AppControl
{
public:
    // all
    static unsigned int scr_width, scr_height;
    static mat4 view, projection, vp;
    static float deltaTime;
    // text
    static float thickness,
        softness, outline_thickness, outline_softness;
    static vec3 text_color;
    static vec2 text_shadow_offset;
    // pbr ibl
    static vec3 irradiance_color;
};

#endif
