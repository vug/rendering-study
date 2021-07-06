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

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    ImGuiLayer::Init(window);

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

        // OpenGL Render
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor((GLfloat)fmod(glfwGetTime(), 1.0), 1.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGuiLayer::End();
        glfwSwapBuffers(window);
    }

    ImGuiLayer::Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}