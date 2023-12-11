#ifndef __SCENE_H__
#define __SCENE_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "resource.h"
#include "objectrender.h"
#include "model.h"

using namespace glm;

const float CubesVertices[] = {
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

const float FrameVertices[] = {
    // positions        // texture Coords
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

const u32string u32text_string(U"使徒来袭 \nmashiro-真白-ましろ ❀ ⛅ ✯ ❅\n ᕕ(◠ڼ◠)ᕗ Ciallo～(∠・ω< )⌒★ (ᗜ˰ᗜ)\n 天动万象 I will have order 𒆚 𒆚 𒆙");
const u32string u32text_string1(U"\n\
 ⠀⠀⠀⠀⠀⠀⠀⠀⣀⡤⠶⠚⡲⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
 ⠀⠀⠀⠀⠀⠀⠀⠸⣧⣀⡶⣦⠘⣽⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀\n\
 ⠀⠀⠀⠀⠀⠀⣀⠤⠟⢛⣿⠉⣾⢭⣀⡉⣳⠀⠀⠀⠀⠀⠀⠀\n\
 ⠀⠀⠀⠀⡰⠋⣁⡴⠛⠉⢹⣟⣻⠀⣴⠶⠃⠀⠀⠀⠀⠀⠀⠀\n\
 ⠀⠀⠀⡼⠁⣞⢁⡠⠔⠒⠉⠉⠉⠉⠙⠲⡤⣀⠀⠀⡀⠀⠀⠀\n\
    ⠈⢉⠟⠉⠀⠀⠀⠀⠀⠀⠀⡀⠀⠘⣦⣉⠩⡇⠀⠀⡀\n\
 ⠀⠀⠀⣠⠃⠀⠀⠀⢀⣠⡀⠀⠀⠀⠸⡢⡠⢿⣿⡇⠀⠈⡹⠁\n\
 ⢠⡒⠋⠁⠀⡀⠀⠀⢀⠇⡵⣖⠀⠀⡀⢇⠉⠢⡉⢳⠀⢏⠁⠀\n\
 ⠀⠉⢒⠖⢠⠀⠀⠀⡟⠀⣀⣈⠑⠢⠽⠝⢀⣶⣜⡄⠀⠈⡆⠀\n\
 ⠀⢀⡏⠀⡄⠀⠀⠈⡇⠀⠿⠿⠃⡀⠀⡀⡀⠙⠉⠸⡄⠀⢸⠀\n\
 ⠀⢸⠁⠀⡇⠀⠀⠀⢹⠀⠀⠜⠀⠈⠉⠀⠀⠀⠀⣠⠇⠀⠈⡆\n\
 ⠀⢸⡄⠀⡇⠀⠀⠀⠘⣦⣀⣠⣤⣤⡴⠶⣶⣶⡿⣿⠁⡀⠀⡇\n\
 ⠀⠀⢳⡄⢳⡀⠀⢠⠀⠹⣿⡾⠘⠖⠱⣶⣴⡆⣹⣧⣾⠃⣸⠃\n\
 ⠀⠀⠀⠉⠳⠿⠶⣬⣿⣶⣾⣿⣦⣓⠘⢋⣛⣧⡴⠻⡃⠒⠁⠀\n\
 ⠀⠀⠀⠀⢀⣠⡾⠿⣿⣄⣤⢹⡎⠉⠉⠍⠉⠁⠠⠀⢱⡀⠀⠀\n\
");

class Scene
{
private:
    Camera *m_camera = nullptr;

    TextObject *text = nullptr;
    // Model *model_Aru = nullptr;
    // Model *model_DamageHelmet = nullptr;
    Model *model_miku = nullptr;
    Model *model_anon = nullptr;
    PBRObject *pbr_model = nullptr;
    SKYObject *skybox = nullptr;

    float zbuffer_near = 0.01;
    float zbuffer_far = 40.0f;

public:
    unsigned int Width, Height;
    Scene(Camera *camera, unsigned int width, unsigned int height);
    Scene();
    ~Scene()
    {
        delete text;
    };
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
};

#endif