#ifndef __CONTEXTCTRL_H__
#define __CONTEXTCTRL_H__

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef RES_DIR
#define RES_DIR "../res/"
#endif

class AppControl
{
public:
    // all
    static unsigned int scr_width, scr_height;
    static glm::mat4 view, projection, vp;
    static float deltaTime;
    // light
    static glm::vec3 pointLightPos, pointLightColor;
    // text
    static float thickness,
        softness, outline_thickness, outline_softness;
    static glm::vec3 text_color;
    static glm::vec2 text_shadow_offset;
    // pbr ibl
    static glm::vec3 irradiance_color;
    // animation
    static std::vector<std::string> animNames;
    static int animIndex;
    static float playSpeed;
    static float duration;
    static float currentFrame;
    static bool playBackState;
    // shadow
    static float bias_offs;
    static float bias_mids;

};

typedef AppControl App;

#endif
