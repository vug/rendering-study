#include <cmath>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "ImGuiLayer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Renderer.h"
#include "Renderer/OrthographicCamera.h"

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
    window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwSetKeyCallback(window, key_callback);
    
    // OpenGL Init
    glfwMakeContextCurrent(window);
    RendererAPI::PrintInfo();

    ImGuiLayer::Init(window);

    // OpenGL Prepare
    std::string vertexSrc = R"(
        #version 460 core

        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        uniform mat4 u_ViewProjection;

        out vec3 v_Position;
        out vec4 v_Color;

        void main() {
            v_Position = a_Position;
            v_Color = a_Color;
            gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
        }
    )";
    std::string fragmentSrc = R"(
        #version 460 core

        layout(location = 0) out vec4 color;

        in vec3 v_Position;
        in vec4 v_Color;

        void main() {
            color = vec4(v_Position * 0.5 + 0.5, 1.0);
            color = v_Color;
        }
    )";
    std::shared_ptr<Shader> shader;
    shader.reset(new Shader(vertexSrc, fragmentSrc));

    std::string blueShaderVertexSrc = R"(
        #version 460 core

        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_ViewProjection;

        out vec3 v_Position;

        void main() {
            v_Position = a_Position;
            gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
        }
    )";
    std::string blueShaderFragmentSrc = R"(
        #version 460 core

        layout(location = 0) out vec4 color;

        in vec3 v_Position;

        void main() {
            color = vec4(0.2, 0.3, 0.8, 1.0);
        }
    )";
    std::shared_ptr<Shader> blueShader;
    blueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));

    std::shared_ptr<VertexArray> vertexArray;
    vertexArray.reset(new VertexArray());

    std::shared_ptr<VertexBuffer> vertexBuffer;
    float vertices[3 * 7] = {
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
    };
    vertexBuffer.reset(new VertexBuffer(vertices, sizeof(vertices)));
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" },
    };
    vertexBuffer->SetLayout(layout);
    vertexArray->AddVertexBuffer(vertexBuffer);
    

    std::shared_ptr<IndexBuffer> indexBuffer;
    uint32_t indices[3] = { 0, 1, 2 };
    indexBuffer.reset(new IndexBuffer(indices, sizeof(indices) / sizeof(uint32_t)));
    vertexArray->SetIndexBuffer(indexBuffer);

    std::shared_ptr<VertexArray> squareVA;
    squareVA.reset(new VertexArray());
    float squareVertices[3 * 4] = {
        -0.75f, -0.75f, 0.0f,
         0.75f, -0.75f, 0.0f,
         0.75f,  0.75f, 0.0f,
        -0.75f,  0.75f, 0.0f,
    };
    std::shared_ptr<VertexBuffer> squareVB;
    squareVB.reset(new VertexBuffer(squareVertices, sizeof(squareVertices)));
    squareVB->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
    });
    squareVA->AddVertexBuffer(squareVB);

    uint32_t squareIndices[3 * 2] = { 
        0, 1, 2, 
        2, 3, 0 
    };
    std::shared_ptr<IndexBuffer> squareIB;
    squareIB.reset(new IndexBuffer(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
    squareVA->SetIndexBuffer(squareIB);

    //OrthographicCamera camera(-1.0f, 1.0f, -1.0f, 1.0f);
    OrthographicCamera camera(-1.6f, 1.6f, -0.9, 0.9f);
    camera.SetRotation(0.0f);
    camera.SetPosition({ 0.0f, 0.0f, 0.0f });
    float cameraMoveSpeed = 0.1f;
    float cameraRotationSpeed = 2.0f;

    bool showDemoWindow = false;
    while (!glfwWindowShouldClose(window))
    {      
        glfwPollEvents();
        ImGuiLayer::Begin();

        auto left = glfwGetKey(window, GLFW_KEY_LEFT);
        auto right = glfwGetKey(window, GLFW_KEY_RIGHT);
        auto up = glfwGetKey(window, GLFW_KEY_UP);
        auto down = glfwGetKey(window, GLFW_KEY_DOWN);

        if (left == GLFW_PRESS || left == GLFW_REPEAT) {
            auto p = camera.GetPosition();
            p.x -= cameraMoveSpeed;
            camera.SetPosition(p);
        }
        else if (right == GLFW_PRESS || right == GLFW_REPEAT) {
            auto p = camera.GetPosition();
            p.x += cameraMoveSpeed;
            camera.SetPosition(p);
        }

        if (up == GLFW_PRESS || left == GLFW_REPEAT) {
            auto p = camera.GetPosition();
            p.y += cameraMoveSpeed;
            camera.SetPosition(p);
        }
        else if (down == GLFW_PRESS || left == GLFW_REPEAT) {
            auto p = camera.GetPosition();
            p.y -= cameraMoveSpeed;
            camera.SetPosition(p);
        }

        auto a_key = glfwGetKey(window, GLFW_KEY_A);
        if (a_key == GLFW_PRESS || a_key == GLFW_REPEAT) {
            auto r = camera.GetRotation();
            r += cameraRotationSpeed;
            camera.SetRotation(r);
        }

        auto d_key = glfwGetKey(window, GLFW_KEY_D);
        if (d_key == GLFW_PRESS || d_key == GLFW_REPEAT) {
            auto r = camera.GetRotation();
            r -= cameraRotationSpeed;
            camera.SetRotation(r);
        }

        // ImGui UI
        if(showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);
        ImGui::Begin("My name is window, ImGui window");
        ImGui::Text("Hello There!");
        ImGui::Checkbox("Show demo window", &showDemoWindow);
        ImGui::End();

        // OpenGL Begin
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        //glViewport(0, 0, width, height); // helps with resize

        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        RenderCommand::Clear();

        // OpenGL Render
        Renderer::BeginScene(camera);

        Renderer::Submit(blueShader, squareVA);
        Renderer::Submit(shader, vertexArray);

        Renderer::EndScene();

        ImGuiLayer::End();
        glfwSwapBuffers(window);
    }

    ImGuiLayer::Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}