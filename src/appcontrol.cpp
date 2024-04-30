#include "appcontrol.h"

unsigned int AppControl::scr_width = 1920;
unsigned int AppControl::scr_height = 1080;
glm::mat4 AppControl::view = glm::mat4(1.0);
glm::mat4 AppControl::projection = glm::mat4(1.0);
glm::mat4 AppControl::vp = glm::mat4(1.0);
float AppControl::deltaTime = 0.0f;

glm::vec3 AppControl::pointLightPos = glm::vec3(.0f);
glm::vec3 AppControl::pointLightColor = glm::vec3(1.0f);

float AppControl::thickness = 0.5;
float AppControl::softness = 0.1;
float AppControl::outline_thickness = 0.5;
float AppControl::outline_softness = 0.01;
glm::vec3 AppControl::text_color = glm::vec3(0.2, 0.8, 0.9);
glm::vec2 AppControl::text_shadow_offset = glm::vec2(2.0, 2.0);

glm::vec3 AppControl::irradiance_color = glm::vec3(1.0);

std::vector<std::string> AppControl::animNames = {""};
int AppControl::animIndex = 0;
float AppControl::playSpeed = 1.0f;
float AppControl::duration = .0f;
float AppControl::currentFrame = .0f;
bool AppControl::playBackState = false;

float AppControl::bias_offs = 0.2f;
float AppControl::bias_mids = 12.0f;