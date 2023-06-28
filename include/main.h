#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;
const unsigned int SCR_WIDTH = 400;
const unsigned int SCR_HEIGHT = 400;

float key_value = 1.0;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
