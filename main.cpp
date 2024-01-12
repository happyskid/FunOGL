#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Camera.hpp"
#include "Shader.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "Model.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // Setup glfw window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FunOGL", NULL, NULL);
    
    // Failed
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return 1;
    }


    // Setup shader
    Shader basicShader("shaders/basic.vert", "shaders/basic.frag");
    Shader lightShader("shaders/light.vert", "shaders/light.frag");

    // Objects
    Model object("models/skull-downloadable/scene.gltf", false);

    GLfloat lightVertices[] =
    { //     COORDINATES     //
        -0.1f, -0.1f,  0.1f,
        -0.1f, -0.1f, -0.1f,
         0.1f, -0.1f, -0.1f,
         0.1f, -0.1f,  0.1f,
        -0.1f,  0.1f,  0.1f,
        -0.1f,  0.1f, -0.1f,
         0.1f,  0.1f, -0.1f,
         0.1f,  0.1f,  0.1f
    };

    GLuint lightIndices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 4, 7,
        0, 7, 3,
        3, 7, 6,
        3, 6, 2,
        2, 6, 5,
        2, 5, 1,
        1, 5, 4,
        1, 4, 0,
        4, 5, 6,
        4, 6, 7
    };

    // Light cube
    VAO lightVAO;
    lightVAO.Bind();
    VBO lightVBO(lightVertices, sizeof(lightVertices));
    EBO lightEBO(lightIndices, sizeof(lightIndices));
    lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(1.0f, 3.0f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    glm::vec3 objectPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 objectModel = glm::mat4(1.0f);
    objectModel = glm::translate(objectModel, objectPos);
    objectModel = glm::rotate(objectModel, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

    lightShader.use();
    lightShader.setMat4("model", lightModel);
    lightShader.setVec4("lightColor", lightColor);
    basicShader.use();
    basicShader.setMat4("model", objectModel);
    basicShader.setVec4("lightColor", lightColor);
    basicShader.setVec3("lightPos", lightPos);

    // Setup buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    Camera camera(SCR_HEIGHT, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 2.5f));

    double oldTime = 0;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.01f, 100.0f);

        double time = glfwGetTime();
        double deltaTime = time - oldTime;
        oldTime = time;

        objectModel = glm::rotate(objectModel, (float)deltaTime, glm::vec3(0.0f, 0.0f, 0.1f));

        basicShader.use();
        basicShader.setVec3("camPos", camera.Position);
        basicShader.setMat4("model", objectModel);
        camera.Matrix(basicShader, "camMatrix");
        object.Draw(basicShader);

        // Draw basic light cube
        lightShader.use();
        camera.Matrix(lightShader, "camMatrix");
        lightVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    basicShader.Delete();
    lightVAO.Delete();
    lightVBO.Delete();
    lightEBO.Delete();
    lightShader.Delete();
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}