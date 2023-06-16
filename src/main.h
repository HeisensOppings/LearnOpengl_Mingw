#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "link.h"
#include <iostream>
using namespace std;
const unsigned int SCR_WIDTH = 400;
const unsigned int SCR_HEIGHT = 300;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void procesInput(GLFWwindow *window);