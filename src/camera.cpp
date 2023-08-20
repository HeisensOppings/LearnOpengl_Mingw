#include "camera.h"

glm::mat4 Camera::GetViewMatrix()
{

    return calculate_lookAt_matrix(m_cameraPos, m_cameraDir, m_cameraUp);
    // return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraDir, m_cameraUp);
}

void Camera::ProcessKeyBoard(CameraMovement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    glm::vec3 move_vector(0.0f, 0.0f, 0.0f);

    switch (direction)
    {
    case MOVE_FORWARD:
        move_vector = -glm::normalize(glm::cross(glm::cross(m_cameraDir, m_cameraUp), m_cameraUp));
        // move_vector = m_cameraDir;
        break;
    case MOVE_BACKWARD:
        move_vector = glm::normalize(glm::cross(glm::cross(m_cameraDir, m_cameraUp), m_cameraUp));
        // move_vector = -m_cameraDir;
        break;
    case MOVE_LEFT:
        move_vector = -glm::normalize(glm::cross(m_cameraDir, m_cameraUp));
        break;
    case MOVE_RIGHT:
        move_vector = glm::normalize(glm::cross(m_cameraDir, m_cameraUp));
        break;
    case MOVE_UP:
        move_vector = m_cameraUp;
        // move_vector = glm::normalize(glm::cross(glm::normalize(glm::cross(m_cameraDir, m_cameraUp)), m_cameraDir));
        break;
    case MOVE_DOWN:
        move_vector = -m_cameraUp;
        // move_vector = -glm::normalize(glm::cross(glm::normalize(glm::cross(m_cameraDir, m_cameraUp)), m_cameraDir));
        break;
    default:
        break;
    }

    move_vector *= velocity;
    m_cameraPos += move_vector;
}

// void Camera::ProcessKeyBoard(CameraMovement direction, float deltaTime)
// {
//     float velocity = MovementSpeed * deltaTime;
//     glm::vec3 move_vector(0.0f, 0.0f, 0.0f);

//     switch (direction)
//     {
//     case MOVE_FORWARD:
//         move_vector.z -= m_cameraFront;
//         break;
//     case MOVE_BACKWARD:
//         move_vector.z += 1.0f;
//         break;
//     case MOVE_LEFT:
//         move_vector.x -= 1.0f;
//         break;
//     case MOVE_RIGHT:
//         move_vector.x += 1.0f;
//         break;
//     case MOVE_UP:
//         move_vector.y += 1.0f;
//         break;
//     case MOVE_DOWN:
//         move_vector.y -= 1.0f;
//         break;
//     default:
//         break;
//     }

//     move_vector = glm::normalize(move_vector) * velocity;
//     m_cameraPos += move_vector;
// }

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    if (constrainPitch)
    {
        m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    m_Fov -= (float)yoffset;
    if (m_Fov < 1.0f)
        m_Fov = 1.0f;
    if (m_Fov > 45.0f)
        m_Fov = 45.0f;
}

void Camera::updateCameraVectors()
{
    m_cameraDir.x = cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
    m_cameraDir.y = sin(glm::radians(m_Pitch));
    m_cameraDir.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));
    // m_cameraDir = glm::normalize(direction);
}

void Camera::loadCameraPosition(GLFWwindow *window)
{
    std::ifstream file("config.ini");
    if (file.is_open())
    {
        int SCR_X_POS = 0, SCR_Y_POS = 0;
        float posX,
            posY, posZ, dirX, dirY, dirZ, upX, upY, upZ, yaw, pitch, fov;
        file >> posX >> posY >> posZ;
        file >> dirX >> dirY >> dirZ;
        file >> upX >> upY >> upZ;
        file >> yaw;
        file >> pitch;
        file >> fov;
        file >> SCR_X_POS;
        file >> SCR_Y_POS;
        glfwSetWindowPos(window, SCR_X_POS, SCR_Y_POS);
        file.close();
        SetCameraSettings(posX, posY, posZ, dirX, dirY, dirZ, upX, upY, upZ, yaw, pitch, fov);
    }
}

void Camera::saveCameraPosition(GLFWwindow *window)
{
    std::ofstream file("config.ini");
    if (file.is_open())
    {
        int SCR_X_POS = 0, SCR_Y_POS = 0;
        file << m_cameraPos.x << " " << m_cameraPos.y << " " << m_cameraPos.z << "\n";
        file << m_cameraDir.x << " " << m_cameraDir.y << " " << m_cameraDir.z << "\n";
        file << m_cameraUp.x << " " << m_cameraUp.y << " " << m_cameraUp.z << "\n";
        file << m_Yaw << "\n";
        file << m_Pitch << "\n";
        file << m_Fov << "\n";
        glfwGetWindowPos(window, &SCR_X_POS, &SCR_Y_POS);
        file << SCR_X_POS << "\n";
        file << SCR_Y_POS << "\n";
        file.close();
    }
}

// Custom implementation of the LookAt function
glm::mat4 Camera::calculate_lookAt_matrix(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
{
    // 1. Position = known
    // 2. Calculate cameraDirection
    glm::vec3 zaxis = glm::normalize(-target);
    // 3. Get positive right axis vector
    glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(worldUp), zaxis));
    // 4. Calculate camera up vector
    glm::vec3 yaxis = glm::cross(zaxis, xaxis);

    // Create translation and rotation matrix
    // In glm we access elements as mat[col][row] due to column-major layout
    glm::mat4 translation = glm::mat4(1.0f); // Identity matrix by default
    translation[3][0] = -position.x;         // Third column, first row
    translation[3][1] = -position.y;
    translation[3][2] = -position.z;
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0][0] = xaxis.x; // First column, first row
    rotation[1][0] = xaxis.y;
    rotation[2][0] = xaxis.z;
    rotation[0][1] = yaxis.x; // First column, second row
    rotation[1][1] = yaxis.y;
    rotation[2][1] = yaxis.z;
    rotation[0][2] = zaxis.x; // First column, third row
    rotation[1][2] = zaxis.y;
    rotation[2][2] = zaxis.z;

    // Return lookAt matrix as combination of translation and rotation matrix
    return rotation * translation; // Remember to read from right to left (first translation then rotation)
}