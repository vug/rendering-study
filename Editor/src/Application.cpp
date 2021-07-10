#include <iostream>

#include "Application.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "ImGuiLayer.h"
#include "Renderer/Renderer.h"

Application::Application(std::string name) 
    : name(name) { }

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    std::cout << "x: " << xoffset << ", " << "y: " << yoffset << std::endl;
    auto app = (Application*)glfwGetWindowUserPointer(window);
    for (auto listener : app->scrollListeners) {
        listener->OnScrollUpdate(xoffset, yoffset);
    }
}

void Application::RegisterScrollListener(ScrollListener* listener) {
    scrollListeners.push_back(listener);
}

int Application::Run() {
    if (!glfwInit()) return -1;
    window = glfwCreateWindow(1280, 720, name.c_str(), NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, Application::scrollCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync enabled. (0 for disabling)

    Renderer::Init();
    RenderCommand::PrintInfo();

    ImGuiLayer::Init(window);

    OnInit();
    
    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        Timestep timestep = time - lastFrameTime;
        framesPerSecond = 1.0f / timestep;
        lastFrameTime = time;

        glfwPollEvents();
        ImGuiLayer::Begin();
        OnImGuiRender();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        //glViewport(0, 0, width, height); // helps with resize
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        RenderCommand::Clear();

        OnUpdate(timestep);

        ImGuiLayer::End();
        glfwSwapBuffers(window);
    }

    OnShutdown();
    ImGuiLayer::Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

bool Application::IsKeyHeld(int key) {
    int keyState = glfwGetKey(window, key);
    return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
}