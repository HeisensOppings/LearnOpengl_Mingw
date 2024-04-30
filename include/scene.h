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
#include "animation.h"

#define DELETE_PTR(param) \
    if (param != nullptr) \
        delete param;

class Scene
{
public:
    Camera *sceneCamera = nullptr;

    float sceneNearPlane = 0.001;
    float sceneFarPlane = 50.0f;
    unsigned int sceneWidth, sceneHeight;

    Scene(Camera *camera, unsigned int scr_width, unsigned int scr_height)
        : sceneWidth(scr_width), sceneHeight(scr_height)
    {
        sceneCamera = camera;
        App::projection = glm::perspective(glm::radians(sceneCamera->m_Fov), (float)sceneWidth / (float)sceneHeight, sceneNearPlane, sceneFarPlane);
        App::view = sceneCamera->GetViewMatrix();
        App::vp = App::projection * App::view;

        // --------------------------------------------------- shader file
        // ResourceManager::ShaderInit(std::string(RES_DIR) + "shaders.glsl");
    }

    virtual ~Scene()
    {
        ResourceManager::Clear();
    }

    virtual void Update(float dt)
    {
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        App::projection = glm::perspective(glm::radians(sceneCamera->m_Fov), (float)sceneWidth / (float)sceneHeight, sceneNearPlane, sceneFarPlane);
        App::view = sceneCamera->GetViewMatrix();
        App::vp = App::projection * App::view;
        App::deltaTime = dt;
    }
    virtual void processInput([[maybe_unused]] GLFWwindow *window) {}
    virtual void Render() {}
};

#endif