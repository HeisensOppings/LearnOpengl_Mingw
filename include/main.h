#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <wchar.h>
#include <locale>
#include <codecvt>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "ft2build.h"
#include FT_FREETYPE_H

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

#define FONT_SIZE 30
// U -- UTF-32; L -- UTF-16; u8 -- UTF-8;
vec3 text_color(0.2, 0.8, 0.9);
string text_string("mashiro-真白-ましろ ❀ ⛅ ✯ ❅\nᕕ(◠ڼ◠)ᕗ Ciallo～(∠・ω< )⌒★ (ᗜ˰ᗜ)\n天动万象 I will have order 𒆚 𒆚 𒆙");
char text_buffer[4096]{"mashiro-真白-ましろ ❀ ⛅ ✯ ❅\nᕕ(◠ڼ◠)ᕗ Ciallo～(∠・ω< )⌒★ (ᗜ˰ᗜ)\n天动万象 I will have order 𒆚 𒆚 𒆙"};
u32string u32text_string(U"mashiro-真白-ましろ ❀ ⛅ ✯ ❅\nᕕ(◠ڼ◠)ᕗ Ciallo～(∠・ω< )⌒★ (ᗜ˰ᗜ)\n天动万象 I will have order 𒆚 𒆚 𒆙");
float text_size(0.01);
bool text_changed = false;
int SDF_Mode = 0;
float line_spec = 8;
float word_spec = 0;
bool hasSDF = true;
double rotationAngle = 30.0;

vector<string> font_paths = {
    "./src/fonts/HarmonyOS_Sans_SC_Medium.ttf",
    "./src/fonts/NotoSansArabic-Medium.ttf",
    "./src/fonts/NotoSansCanadianAboriginal-Medium.ttf",
    "./src/fonts/NotoSansCuneiform-Regular.ttf",
    "./src/fonts/NotoSansSymbols2-Regular.ttf",
    "./src/fonts/NotoSans-ExtraBold.ttf",
};

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
bool gamma = false;
bool hasSSAO = true;

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
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

/// Holds all state information relevant to a character as loaded using FreeType
struct Character
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};
std::map<GLchar, Character> Characters;
unsigned int VAO, VBO;

int opengl_init();
// void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void SceneLightConfig(Shader &shader, glm::mat4 view, glm::mat4 projuction);

// GLenum glCheckError_(const char *file, int line)
// {
//     GLenum errorCode;
//     while ((errorCode = glGetError()) != GL_NO_ERROR)
//     {
//         std::string error;
//         switch (errorCode)
//         {
//         case GL_INVALID_ENUM:
//             error = "INVALID_ENUM";
//             break;
//         case GL_INVALID_VALUE:
//             error = "INVALID_VALUE";
//             break;
//         case GL_INVALID_OPERATION:
//             error = "INVALID_OPERATION";
//             break;
//         // case GL_STACK_OVERFLOW:
//             // error = "STACK_OVERFLOW";
//             // break;
//         // case GL_STACK_UNDERFLOW:
//             // error = "STACK_UNDERFLOW";
//             // break;
//         case GL_OUT_OF_MEMORY:
//             error = "OUT_OF_MEMORY";
//             break;
//         case GL_INVALID_FRAMEBUFFER_OPERATION:
//             error = "INVALID_FRAMEBUFFER_OPERATION";
//             break;
//         }
//         std::cout << error << " | " << file << " (" << line << ")" << std::endl;
//     }
//     return errorCode;
// }
// #define glCheckError() glCheckError_(__FILE__, __LINE__)
