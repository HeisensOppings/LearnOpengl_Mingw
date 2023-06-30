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
    Shader Program2(1, 1);

    float vertices1[] = {
        // positions          // colors           // texture coords (note that we changed them to 2.0f!)
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

    float vertices2[] = {
        // positions      // texture coords (note that we changed them to 2.0f!)
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,  // top left
    };
    // int indices[] = {1, 2, 2, 3, 3, 1};//GL_LINES
    int indices[] = {0, 1, 3, 1, 2, 3};

    vector<int> layout_stride1{3, 3, 2};
    BufferLayout layout1;
    layout1.AddFloat(layout_stride1);

    VertexArray VAO1;
    VertexBuffer VBO1(vertices1, sizeof(vertices1));
    IndexBuffer IBO1(indices, 6);
    VAO1.AddBuffer(VBO1, layout1);

    vector<int> layout_stride2{3, 3};
    BufferLayout layout2;
    layout2.AddFloat(layout_stride2);

    VertexArray VAO2;
    VertexBuffer VBO2(vertices2, sizeof(vertices2));
    IndexBuffer IBO2(indices, 6);
    VAO2.AddBuffer(VBO2, layout2);

    Texture texture1("E:/Project/OpenGL/src/image/texture1.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
    Texture texture2("E:/Project/OpenGL/src/image/texture2.png", GL_REPEAT, GL_LINEAR);

    Program1.Bind();
    Program1.SetUniform1i("texture1", 0);
    Program1.SetUniform1i("texture2", 1);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Program1.Bind();
        glActiveTexture(GL_TEXTURE0);
        texture1.Bind();
        glActiveTexture(GL_TEXTURE1);
        texture2.Bind();

        Program1.SetUniform1f("scale", key_value);
        Program1.SetUniform2f("view_position", key_value_x, key_value_y);
        Program1.SetUniform1i("switchTexture", switchTexture);

        unsigned int transformLocation = glGetUniformLocation(Program1.GetID(), "transform");
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(-0.5f, 0.5f, 0.0f)); // Switched the order
        // transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)); // Switched the order
        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        Program2.Bind();
        unsigned int transformLocation1 = glGetUniformLocation(Program2.GetID(), "transform");
        glm::mat4 transform1(1.0f);
        transform1 = glm::translate(transform1, glm::vec3(0.5f, -0.5f, 0.0f)); // Switched the order
        // transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)); // Switched the order
        glUniformMatrix4fv(transformLocation1, 1, GL_FALSE, glm::value_ptr(transform1));
        VAO2.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
