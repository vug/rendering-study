#include <iostream>
#include <string>

#include "GLFW/glfw3.h"
#include "imgui/imgui.h"

#include "Editor.h"

#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"

Editor::Editor() : Application("Ugur's Editor") { }

void Editor::OnInit() {
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
    blueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
        
    vertexArray.reset(new VertexArray());  
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

    uint32_t indices[3] = { 0, 1, 2 };
    indexBuffer.reset(new IndexBuffer(indices, sizeof(indices) / sizeof(uint32_t)));
    vertexArray->SetIndexBuffer(indexBuffer);

    squareVA.reset(new VertexArray());
    float squareVertices[3 * 4] = {
        -0.75f, -0.75f, 0.0f,
         0.75f, -0.75f, 0.0f,
         0.75f,  0.75f, 0.0f,
        -0.75f,  0.75f, 0.0f,
    };
    
    squareVB.reset(new VertexBuffer(squareVertices, sizeof(squareVertices)));
    squareVB->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        });
    squareVA->AddVertexBuffer(squareVB);

    uint32_t squareIndices[3 * 2] = {
        0, 1, 2,
        2, 3, 0
    };
    
    squareIB.reset(new IndexBuffer(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
    squareVA->SetIndexBuffer(squareIB);
}

void Editor::OnImGuiRender() {
    if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);
    ImGui::Begin("My name is window, ImGui window");
    std::string fps = std::string("FPS: ") + std::to_string(framesPerSecond);
    ImGui::Text(fps.c_str());
    ImGui::Checkbox("Show demo window", &showDemoWindow);
    ImGui::End();
}

void Editor::OnUpdate(Timestep ts) {
    auto left = glfwGetKey(window, GLFW_KEY_LEFT);
    auto right = glfwGetKey(window, GLFW_KEY_RIGHT);
    auto up = glfwGetKey(window, GLFW_KEY_UP);
    auto down = glfwGetKey(window, GLFW_KEY_DOWN);

    if (left == GLFW_PRESS || left == GLFW_REPEAT) {
        auto p = camera.GetPosition();
        p.x -= cameraMoveSpeed * ts;
        camera.SetPosition(p);
    }
    else if (right == GLFW_PRESS || right == GLFW_REPEAT) {
        auto p = camera.GetPosition();
        p.x += cameraMoveSpeed * ts;
        camera.SetPosition(p);
    }

    if (up == GLFW_PRESS || left == GLFW_REPEAT) {
        auto p = camera.GetPosition();
        p.y += cameraMoveSpeed * ts;
        camera.SetPosition(p);
    }
    else if (down == GLFW_PRESS || left == GLFW_REPEAT) {
        auto p = camera.GetPosition();
        p.y -= cameraMoveSpeed * ts;
        camera.SetPosition(p);
    }

    auto a_key = glfwGetKey(window, GLFW_KEY_A);
    if (a_key == GLFW_PRESS || a_key == GLFW_REPEAT) {
        auto r = camera.GetRotation();
        r += cameraRotationSpeed * ts;
        camera.SetRotation(r);
    }

    auto d_key = glfwGetKey(window, GLFW_KEY_D);
    if (d_key == GLFW_PRESS || d_key == GLFW_REPEAT) {
        auto r = camera.GetRotation();
        r -= cameraRotationSpeed * ts;
        camera.SetRotation(r);
    }

    Renderer::BeginScene(camera);
    Renderer::Submit(blueShader, squareVA);
    Renderer::Submit(shader, vertexArray);
    Renderer::EndScene();
}

void Editor::OnShutdown() {

}