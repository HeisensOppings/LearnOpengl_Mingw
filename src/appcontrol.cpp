#include "appcontrol.h"

unsigned int AppControl::scr_width = 800;
unsigned int AppControl::scr_height = 600;
mat4 AppControl::view = mat4(1.0);
mat4 AppControl::projection = mat4(1.0);
mat4 AppControl::vp = mat4(1.0);
float AppControl::deltaTime = 0.0f;

float AppControl::thickness = 0.5;
float AppControl::softness = 0.1;
float AppControl::outline_thickness = 0.5;
float AppControl::outline_softness = 0.01;
vec3 AppControl::text_color = vec3(0.2, 0.8, 0.9);
vec2 AppControl::text_shadow_offset = vec2(2.0, 2.0);

vec3 AppControl::irradiance_color = vec3(1.0);
