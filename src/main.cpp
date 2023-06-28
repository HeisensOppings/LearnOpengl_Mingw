#include "main.h"
int main()
{
    if (opengl_init() != 0)
    {
        cout << "Failed to initialize OpenGL" << endl;
        return -1;
    }

    const string filepath = "E:/Project/OpenGL/src/shaders.glsl";
    Shader::ShaderInit(filepath);
    Shader Program1(0, 0);

    float vertices1[] = {
        // positions          // colors           // texture coords (note that we changed them to 2.0f!)
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

    // int indices[] = {1, 2, 2, 3, 3, 1};//GL_LINES
    int indices[] = {0, 1, 3, 1, 2, 3};
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

    Texture texture1("E:/Project/OpenGL/src/image/texture1.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
    Texture texture2("E:/Project/OpenGL/src/image/texture2.png", GL_REPEAT, GL_LINEAR);

    Program1.Bind();
    Program1.SetUniform1i("texture1", 0);
    Program1.SetUniform1i("texture2", 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float timeValue = glfwGetTime();
        float sinValue = sin(timeValue) / 2.0f;
        float cosValue = cos(timeValue) / 2.0f;

        Program1.Bind();
        glActiveTexture(GL_TEXTURE0);
        texture1.Bind();
        glActiveTexture(GL_TEXTURE1);
        texture2.Bind();

        Program1.SetUniform2f("xyOffset", sinValue, cosValue);
        Program1.SetUniform1f("scale", key_value);
        Program1.SetUniform2f("view_position", key_value_x, key_value_y);
        Program1.SetUniform1i("switchTexture", switchTexture);

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
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        transform = glm::mat4(1.0f); // reset it to identity matrix
        transform = glm::translate(transform, glm::vec3(0.5f, 0.5f, 0.0f));
        float scaleAmount = 1;
        // float scaleAmount = static_cast<float>(sin(glfwGetTime()));
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
