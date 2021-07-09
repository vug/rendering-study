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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::cout << "key: " << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int Application::Run() {
    if (!glfwInit()) return -1;
    window = glfwCreateWindow(1280, 720, name.c_str(), NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync enabled. (0 for disabling)
    RendererAPI::PrintInfo();
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