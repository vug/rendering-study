#include <cmath>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h" // All headers will be included by glad
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "ImGuiLayer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"

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
    window = glfwCreateWindow(1024, 1024, "Hello World", NULL, NULL);
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
    std::string vertexSrc = R"(
        #version 460 core

        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        out vec3 v_Position;
        out vec4 v_Color;

        void main() {
            v_Position = a_Position;
            v_Color = a_Color;
            gl_Position = vec4(a_Position, 1.0);
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

        out vec3 v_Position;

        void main() {
            v_Position = a_Position;
            gl_Position = vec4(a_Position, 1.0);
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
        blueShader->Bind();
        squareVA->Bind();
        glDrawElements(GL_TRIANGLES, squareVA->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
        shader->Bind();
        vertexArray->Bind();
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

        ImGuiLayer::End();
        glfwSwapBuffers(window);
    }

    ImGuiLayer::Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}