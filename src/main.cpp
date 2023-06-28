#include "main.h"
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "mashiro", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetWindowPos(window, 300, 640);

    const string filepath = "E:/Project/OpenGL/src/shaders.glsl";
    Shader::ShaderInit(filepath);
    Shader Program1(0, 0);

    float vertices1[] = {
        // positions          // colors           // texture coords (note that we changed them to 2.0f!)
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f, 2.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 2.0f   // top left
    };

    // float vertices2[] = {
    //     0.9f, 0.9f, 0.0f,
    //     0.9, 0.0f, 0.0f,
    //     -0.9f, 0.0f, 0.0f,
    //     -0.9f, 0.0f, 0.0f,
    // };

    // int indices[] = {1, 2, 2, 3, 3, 1};//GL_LINES
    int indices[] = {0,1,3,1,2,3}; 
    unsigned int VAOs[2], VBOs[2], EBO;
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load and create a texture
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load("E:/Project/OpenGL/src/image/texture1.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("E:/Project/OpenGL/src/image/texture2.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    Program1.Bind();
    Program1.SetUniform1i("texture1", 0);
    Program1.SetUniform1i("texture2", 1);

    // glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    // 0.9.9
    // glm::mat4 trans = glm::vec4(1.0f);
    // glm::mat4 trans_rotate, trans_scale, trans;
    // trans_rotate = glm::rotate(trans_rotate, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // // trans_scale = glm::scale(trans_scale, glm::vec3(0.5f, 0.5f, 0.5f));
    // unsigned int transformLocation = glGetUniformLocation(Program1.GetID(), "transform");
    // glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans_rotate));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float timeValue = glfwGetTime();
        float sinValue = sin(timeValue) / 2.0f;
        float cosValue = cos(timeValue)/2.0f;

        Program1.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        Program1.SetUniform2f("xyOffset", sinValue, cosValue);
        Program1.SetUniform1f("alpha", key_value);
        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        unsigned int transformLocation = glGetUniformLocation(Program1.GetID(), "transform");

        glm::mat4 transform;
        // AB != BA
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)); // Switched the order
        transform = glm::translate(transform, glm::vec3(-0.5f, 0.5f, 0.0f));                   // Switched the order
        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        transform = glm::mat4(1.0f); // reset it to identity matrix
        transform = glm::translate(transform, glm::vec3(0.5f, 0.5f, 0.0f));
        float scaleAmount = static_cast<float>(sin(glfwGetTime()));
        transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &transform[0][0]); // this time take the matrix value array's first element as its memory pointer value
        // now with the uniform matrix being replaced with new transformations, draw it again.
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glm::mat4 trans_rotate;
        // // trans_rotate = glm::rotate(trans_rotate, glm::radians(timeValue * 10), glm::vec3(0.0f, 0.0f, 1.0f));
        // trans_rotate = glm::rotate(trans_rotate, glm::radians(timeValue * 10), glm::vec3(0.0f, 0.0f, 1.0f));
        // // trans_scale = glm::scale(trans_scale, glm::vec3(0.5f, 0.5f, 0.5f));
        // glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans_rotate));

        // glfw: swap buffers and poll IO events (keyspressed/released, mouse moved etc.)
        // ---------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents(); 
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);

    // glfw: terminate, clearing all previously allocated GLFWresources.
    //---------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int width, int height)
{   
    // make sure the viewport matches the new window dimensions; note that width and height
    // will be significantly larger than specified on retina displays
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        int width, heigh;
        glfwGetFramebufferSize(window, &width, &heigh);
        framebuffer_size_callback(window, width, heigh);
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        key_value += 0.001f; 
        if (key_value >= 1.0f)
            key_value = 1.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        key_value -= 0.001f;
        if (key_value <= 0.0f)
            key_value = 0.0f;
    }
}