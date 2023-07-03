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
        // positions      // texture coords (note that we changed them to 2.0f!)
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top left
    };

    // float vertices1[] = {
    //     // positions          // colors           // texture coords (note that we changed them to 2.0f!)
    //     0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
    //     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    //     -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    // };

    float vertices3[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    // float vertices2[] = {
    //     // positions      // texture coords (note that we changed them to 2.0f!)
    //     0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // top right
    //     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
    //     -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,  // top left
    // };
    // int indices[] = {1, 2, 2, 3, 3, 1};//GL_LINES
    int indices[] = {0, 1, 3, 1, 2, 3};

    vector<int> layout_stride1{3, 2};
    BufferLayout layout1;
    layout1.AddFloat(layout_stride1);

    VertexArray VAO1;
    VertexBuffer VBO1(vertices3, sizeof(vertices3));
    IndexBuffer IBO1(indices, 6);
    VAO1.AddBuffer(VBO1, layout1);

    Texture texture1("E:/Project/OpenGL/src/image/texture1.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
    Texture texture2("E:/Project/OpenGL/src/image/texture2.png", GL_REPEAT, GL_LINEAR);

    Program1.Bind();
    Program1.SetUniform1i("texture1", 0);
    Program1.SetUniform1i("texture2", 1);

    glEnable(GL_DEPTH_TEST);
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.5f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Program1.Bind();
        glActiveTexture(GL_TEXTURE0);
        texture1.Bind();
        glActiveTexture(GL_TEXTURE1);
        texture2.Bind();

        Program1.SetUniform1f("scale", 1.0f);
        Program1.SetUniform2f("view_position", key_value_x, key_value_y);
        Program1.SetUniform1i("switchTexture", switchTexture);

        // unsigned int transformLocation = glGetUniformLocation(Program1.GetID(), "transform");
        // glm::mat4 transform(1.0f);
        // transform = glm::translate(transform, glm::vec3(0.5f, 0.5f, 0.0f)); // Switched the order
        // // transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)); // Switched the order
        // glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));

        // glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        // model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0, 0.0f, 0.0f));
        view = glm::translate(view, glm::vec3(key_value_x, key_value_y, key_value1));
        projection = glm::perspective(glm::radians(key_value), 1.0f, 0.1f, 100.0f);

        // unsigned int modelLocation = glGetUniformLocation(Program1.GetID(), "model");
        unsigned int viewLocation = glGetUniformLocation(Program1.GetID(), "view");
        // glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
        Program1.SetUniform4m("projection", projection);

        VAO1.Bind();
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
            Program1.SetUniform4m("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
