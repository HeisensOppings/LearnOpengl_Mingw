#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "appcontrol.h"

enum CameraMovement
{
    MOVE_FORWARD = 0,
    MOVE_BACKWARD,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN
};

// default camera angles
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.0f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;
const float Z_VALUE = -3.0f;

class Camera
{
public:
    // camera Attributes
    glm::vec3 m_cameraPos;
    glm::vec3 m_cameraDir;
    glm::vec3 m_cameraUp;
    // euler Angles
    float m_Yaw;
    float m_Pitch;

    float m_Fov;
    // camera options
    float MovementSpeed = SPEED;
    float MouseSensitivity = SENSITIVITY;

    glm::mat4 GetViewMatrix();
    void ProcessKeyBoard(CameraMovement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    void updateCameraVectors();
    inline void SetSpeed(float speed) { MovementSpeed = speed; }
    static glm::mat4 calculate_lookAt_matrix(glm::vec3 cameraPos, glm::vec3 cameraDir, glm::vec3 cameraUp);
    void loadCameraPosition(GLFWwindow *window);
    void saveCameraPosition(GLFWwindow *window);

    Camera(GLFWwindow *window) { loadCameraPosition(window); }

    Camera(glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -Z_VALUE), glm::vec3 cameraDir = glm::vec3(0.0f, 0.0f, Z_VALUE), glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f))
        : m_cameraPos(cameraPos),
          m_cameraDir(cameraDir),
          m_cameraUp(cameraUp),
          m_Yaw(YAW),
          m_Pitch(PITCH),
          m_Fov(FOV)
    {
        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, float dirX, float dirY, float dirZ, float upX, float upY, float upZ, float yaw, float pitch, float fov)
        : m_cameraPos(glm::vec3(posX, posY, posZ)),
          m_cameraDir(glm::vec3(dirX, dirY, dirZ)),
          m_cameraUp(glm::vec3(upX, upY, upZ)),
          m_Yaw(yaw),
          m_Pitch(pitch),
          m_Fov(fov)
    {
        updateCameraVectors();
    }
    void SetCameraSettings(float posX, float posY, float posZ, float dirX, float dirY, float dirZ, float upX, float upY, float upZ, float yaw, float pitch, float fov)
    {
        m_cameraPos = glm::vec3(posX, posY, posZ);
        m_cameraDir = glm::vec3(dirX, dirY, dirZ);
        m_cameraUp = glm::vec3(upX, upY, upZ);
        m_Yaw = yaw;
        m_Pitch = pitch;
        m_Fov = fov;
        updateCameraVectors();
    }
};

#endif