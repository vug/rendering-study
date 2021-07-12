#include <iostream>

#include "Application.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "ImGuiLayer.h"
#include "Renderer/Renderer.h"

Application* Application::instance = nullptr;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

Application::Application(std::string name) 
    : name(name) {
    instance = this;

    assert(glfwInit()); // GLFW initialization failed.
    window = glfwCreateWindow(1280, 720, name.c_str(), NULL, NULL);
    assert(window); // GLFW window creation failed
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, Application::scrollCallback);
    glfwSetWindowSizeCallback(window, Application::windowSizeCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync enabled. (0 for disabling)
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto app = (Application*)glfwGetWindowUserPointer(window);
    for (auto listener : app->scrollListeners) {
        listener->OnScrollUpdate((float)xoffset, (float)yoffset);
    }
}

void Application::windowSizeCallback(GLFWwindow* window, int width, int height) {
    RenderCommand::SetViewport(0, 0, width, height);

    auto app = (Application*)glfwGetWindowUserPointer(window);
    for (auto listener : app->windowListeners) {
        listener->OnWindowResize(width, height);
    }
}

void Application::RegisterScrollListener(ScrollListener* listener) {
    scrollListeners.push_back(listener);
}

void Application::RegisterWindowListener(WindowListener* listener) {
    windowListeners.push_back(listener);
}

int Application::Run() {
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

void Application::Close() {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}