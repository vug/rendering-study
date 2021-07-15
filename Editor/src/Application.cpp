#include <iostream>

#include "Application.h"

#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "ImGuiLayer.h"
#include "Renderer/Renderer.h"

Application* Application::instance = nullptr;

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = (Application*)glfwGetWindowUserPointer(window);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    for (auto listener : app->keyListeners) {
        listener->OnKeyPress(key, action, mods);
    }
}

Application::Application(std::string name) 
    : name(name) {
    instance = this;

    assert(glfwInit()); // GLFW initialization failed.
    window = glfwCreateWindow(1280, 720, name.c_str(), NULL, NULL);
    assert(window); // GLFW window creation failed
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, Application::keyCallback);
    glfwSetScrollCallback(window, Application::scrollCallback);
    glfwSetWindowSizeCallback(window, Application::windowSizeCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync enabled. (0 for disabling)
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto app = (Application*)glfwGetWindowUserPointer(window);
    if (!app->isViewportPaneFocused || !app->isViewportPaneHovered) {
        return;
    }
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

void Application::RegisterKeyListener(KeyListener* listener) {
    keyListeners.push_back(listener);
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

    int w, h;
    glfwGetWindowSize(GetWindow(), &w, &h);
    FramebufferSpecification fbSpec = { (uint32_t)w, (uint32_t)h };
    viewportFramebuffer = std::make_shared<Framebuffer>(fbSpec);

    OnInit();
    
    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        Timestep timestep = time - lastFrameTime;
        framesPerSecond = 1.0f / timestep;
        lastFrameTime = time;

        glfwPollEvents();
        ImGuiLayer::Begin();

        static bool docspaceOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &docspaceOpen, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit")) {
                    Close();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        OnImGuiRender();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport");
        isViewportPaneFocused = ImGui::IsWindowFocused();
        isViewportPaneHovered = ImGui::IsWindowHovered();
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        if (viewportPanelSize.x != viewportSize.x || viewportPanelSize.y != viewportSize.y) {
            viewportFramebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
            viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
            OnViewportResize(viewportPanelSize.x, viewportPanelSize.y);
        }
        uint32_t textureID = viewportFramebuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2{ viewportSize.x, viewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });

        OnImGuiViewportRender();
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End();

        viewportFramebuffer->Bind();
        OnUpdate(timestep);
        viewportFramebuffer->Unbind();

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