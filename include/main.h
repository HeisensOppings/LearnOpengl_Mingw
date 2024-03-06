#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <wchar.h>
#include <locale>
#include <codecvt>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#include "model.h"
#include "shader.h"
#include "texture.h"
#include "vertexarray.h"
#include "camera.h"
// #include "framebuffer.h"
#include "scene.h"

#include "objectrender.h"
#include "appcontrol.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;
using namespace glm;

bool imgui_window_focus = false;

// ⠀⠀⠀⠀⠀⠀⠀⠀⣀⡤⠶⠚⡲⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠸⣧⣀⡶⣦⠘⣽⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⣀⠤⠟⢛⣿⠉⣾⢭⣀⡉⣳⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⡰⠋⣁⡴⠛⠉⢹⣟⣻⠀⣴⠶⠃⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⡼⠁⣞⢁⡠⠔⠒⠉⠉⠉⠉⠙⠲⡤⣀⠀⠀⡀⠀⠀⠀
// ⠀⠀⠀⠈⢉⠟⠉⠀⠀⠀⠀⠀⠀⠀⡀⠀⠘⣦⣉⠩⡇⠀⠀⡀
// ⠀⠀⠀⣠⠃⠀⠀⠀⢀⣠⡀⠀⠀⠀⠸⡢⡠⢿⣿⡇⠀⠈⡹⠁
// ⢠⡒⠋⠁⠀⡀⠀⠀⢀⠇⡵⣖⠀⠀⡀⢇⠉⠢⡉⢳⠀⢏⠁⠀
// ⠀⠉⢒⠖⢠⠀⠀⠀⡟⠀⣀⣈⠑⠢⠽⠝⢀⣶⣜⡄⠀⠈⡆⠀
// ⠀⢀⡏⠀⡄⠀⠀⠈⡇⠀⠿⠿⠃⡀⠀⡀⡀⠙⠉⠸⡄⠀⢸⠀
// ⠀⢸⠁⠀⡇⠀⠀⠀⢹⠀⠀⠜⠀⠈⠉⠀⠀⠀⠀⣠⠇⠀⠈⡆
// ⠀⢸⡄⠀⡇⠀⠀⠀⠘⣦⣀⣠⣤⣤⡴⠶⣶⣶⡿⣿⠁⡀⠀⡇
// ⠀⠀⢳⡄⢳⡀⠀⢠⠀⠹⣿⡾⠘⠖⠱⣶⣴⡆⣹⣧⣾⠃⣸⠃
// ⠀⠀⠀⠉⠳⠿⠶⣬⣿⣶⣾⣿⣦⣓⠘⢋⣛⣧⡴⠻⡃⠒⠁⠀
// ⠀⠀⠀⠀⢀⣠⡾⠿⣿⣄⣤⢹⡎⠉⠉⠍⠉⠁⠠⠀⢱⡀⠀⠀

// U -- UTF-32; L -- UTF-16; u8 -- UTF-8;
string text_string("| mashiro-真白-ましろ ❀ ⛅ ✯ ❅\n ᕕ(◠ڼ◠)ᕗ Ciallo～(∠・ω< )⌒★ (ᗜ˰ᗜ)\n 天动万象 I will have order 𒆚 𒆚 𒆙");
char text_buffer[4096]{"| mashiro-真白-ましろ ❀ ⛅ ✯ ❅\n ᕕ(◠ڼ◠)ᕗ Ciallo～(∠・ω< )⌒★ (ᗜ˰ᗜ)\n 天动万象 I will have order 𒆚 𒆚 𒆙"};
// u32string u32text_string(U"| mashiro-真白-ましろ ❀ ⛅ ✯ ❅\n ᕕ(◠ڼ◠)ᕗ Ciallo～(∠・ω< )⌒★ (ᗜ˰ᗜ)\n 天动万象 I will have order 𒆚 𒆚 𒆙");
// u32string u32text_string(U"Mm Nn 使徒来袭 \nmashiro-真白-ましろ ❀ ⛅ ✯ ❅\n ᕕ(◠ڼ◠)ᕗ Ciallo～(∠・ω< )⌒★ (ᗜ˰ᗜ)\n 天动万象 I will have order 𒆚 𒆚 𒆙");

float text_size(0.01);

glm::vec3 background_color(0.1);

float exposure = 1.0;
bool hdr = 1;
float _fresnel = 0.0;

float zbuffer_near = 0.01;
float zbuffer_far = 50.0f;

glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

glm::vec3 lightPos(0.0f, 2.0f, 0.0f);
// glm::vec3 lightDirection(0.0f, 0.0f, -1.0f);
glm::vec3 lightDirection(0.213315, -0.295711, 0.931156);

glm::vec3 sunlight_color(1.0f);
glm::vec3 sunlight_pos(20.0f, 3.0f, 100.0f);

// glm::vec3 lightSpotPos(-2.0f, 4.0f, -1.0f);
glm::vec3 lightSpotPos(-2.62747, 3.14707, -8.38304);

glm::vec3 light_am_di_sp_directional(0.3f, 0.6f, 0.2f);
glm::vec3 lightColor_point(1.0f);
glm::vec3 light_am_di_sp_point(0.1f, 0.5f, 0.1f);
glm::vec3 lightColor_spot(1.0f);
glm::vec3 light_am_di_sp_spot(0.1f, 0.8f, 0.1f);

unsigned int light_distance_select_point = 2;
unsigned int light_distance_select_spot = 2;
vector<vector<float>> light_distance{{0.14, 0.07}, {0.07, 0.017}, {0.027, 0.0028}, {0.014, 0.0007}, {0.007, 0.0002}};
vector<int> light_distance_index{32, 65, 160, 325, 600};
// ----------------------------air - water ice -- glass diamond
vector<float> refractive_index{1.00, 1.33, 1.309, 1.51, 2.42};
// float refractive_rate = 1.20;

glm::vec3 sphereColor(0.5, 0.0, 0.0);

int material_shininess = 32;

bool lighting_mode_camera = false;
// bool depth_test = false;
// bool gamma = false;
bool hasSSAO = true;

unsigned int VAO, VBO;

int opengl_init();
void imgui_frame();
// void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void SceneLightConfig(Shader &shader, glm::mat4 view, glm::mat4 projuction);
