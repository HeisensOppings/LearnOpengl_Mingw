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
    static float elapsedTime;      // seconds since start (replaces glfwGetTime)
    static unsigned int defaultFBO; // QOpenGLWidget::defaultFramebufferObject()
    static glm::vec3 background_color; // solid clear colour (environment background removed)
    static glm::vec3 environment_color; // constant IBL environment radiance
    // light
    static glm::vec3 pointLightPos, pointLightColor;
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
