#include <cmath>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h" // All headers will be included by glad
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "ImGuiLayer.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::cout << "key: " << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


int main() {
	std::cout << "Welcome to Hackamonth Study Editor!" << std::endl;

    GLFWwindow* window;
    if (!glfwInit()) return -1;
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwSetKeyCallback(window, key_callback);
    
    // OpenGL Init
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    std::cout << "OpenGL Info" << std::endl
        << "  Renderer: " << glGetString(GL_VENDOR) << std::endl
        << "  Vendor: " << glGetString(GL_RENDERER) << std::endl
        << "  Version: " << glGetString(GL_VERSION) << std::endl;

    ImGuiLayer::Init(window);

    // OpenGL Prepare
    unsigned int vertexArray, vertexBuffer, indexBuffer;
    // Vertex Array
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    // Vertex Buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    float vertices[3 * 3] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    // Index Buffer
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    unsigned int indices[3] = { 0, 1, 2 };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    bool showDemoWindow = false;
    while (!glfwWindowShouldClose(window))
    {      
        glfwPollEvents();
        ImGuiLayer::Begin();

        // ImGui UI
        if(showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);
        ImGui::Begin("My name is window, ImGui window");
        ImGui::Text("Hello There!");
        ImGui::Checkbox("Show demo window", &showDemoWindow);
        ImGui::End();

        // OpenGL Begin
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // OpenGL Render
        glBindVertexArray(vertexArray);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

        ImGuiLayer::End();
        glfwSwapBuffers(window);
    }

    ImGuiLayer::Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}