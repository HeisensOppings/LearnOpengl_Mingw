#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "render.h"
#include "model.h"
#include "shader.h"
#include "texture.h"
#include "vertexarray.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "camera.h"
#include "framebuffer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

glm::vec3 background_color(0.1);

float exposure = 1.0;
bool hdr = 1;

float zbuffer_near = 0.1;
float zbuffer_far = 20.0f;

glm::vec3 lightPos(0.0f, 1.0f, 0.0f);
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

int material_shininess = 32;

bool lighting_mode_camera = false;
// bool depth_test = false;
bool gamma = false;

float CubesVertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
    // Front face
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
    // Right face
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-right
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    // Top face
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
};

float FrameVertices[] = {
    // positions        // texture Coords
    -1.0f,1.0f,0.0f,0.0f,1.0f,
    -1.0f,-1.0f,0.0f,0.0f,0.0f,
    1.0f,1.0f,0.0f,1.0f,1.0f,
    1.0f,-1.0f,0.0f,1.0f,0.0f};

int opengl_init();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void SceneLightConfig(Shader &shader, glm::mat4 view, glm::mat4 projuction);
