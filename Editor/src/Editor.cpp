#include <iostream>
#include <string>

#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Editor.h"

#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"

Editor::Editor() : Application("Ugur's Editor"), squarePosition(0.0f) { }

void Editor::OnInit() {
    std::string vertexSrc = R"(
        #version 460 core

        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec3 v_Position;
        out vec4 v_Color;

        void main() {
            v_Position = a_Position;
            v_Color = a_Color;
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
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

    std::string flatColorShaderVertexSrc = R"(
        #version 460 core

        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec3 v_Position;

        void main() {
            v_Position = a_Position;
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
        }
    )";
    std::string flatColorShaderFragmentSrc = R"(
        #version 460 core

        layout(location = 0) out vec4 color;

        in vec3 v_Position;

        uniform vec3 u_Color;

        void main() {
            color = vec4(u_Color, 1.0);
        }
    )";
    flatColorShader.reset(new Shader(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));

    std::string textureShaderVertexSrc = R"(
        #version 460 core

        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec2 a_TexCoord;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec2 v_TexCoord;

        void main() {
            v_TexCoord = a_TexCoord;
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
        }
    )";
    std::string textureShaderFragmentSrc = R"(
        #version 460 core

        layout(location = 0) out vec4 color;

        in vec2 v_TexCoord;

        uniform sampler2D u_Texture;

        void main() {
            color = texture(u_Texture, v_TexCoord);
            //color = vec4(v_TexCoord, 0.0, 1.0);
        }
    )";
    textureShader.reset(new Shader(textureShaderVertexSrc, textureShaderFragmentSrc));
    
    texture.reset(new Texture2D("assets/textures/Checkerboard.png"));
    textureShader->Bind();
    textureShader->UploadUniformInt("u_Texture", diffuseTextureSlot);
       
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
    float squareVertices[(3 + 2) * 4] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
    };
    
    squareVB.reset(new VertexBuffer(squareVertices, sizeof(squareVertices)));
    squareVB->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float2, "a_TexCoord" },
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
    ImGui::Begin("Settings");
    ImGui::ColorEdit3("Square Color", glm::value_ptr(squareColor));

    std::string fps = std::string("FPS: ") + std::to_string(framesPerSecond);
    ImGui::Text(fps.c_str());

    ImGui::Checkbox("Show demo window", &showDemoWindow);
    ImGui::End();
}

void Editor::OnUpdate(Timestep ts) {
    // Camera Control
    if (IsKeyHeld(GLFW_KEY_LEFT)) cameraPosition.x -= cameraMoveSpeed * ts;
    else if (IsKeyHeld(GLFW_KEY_RIGHT)) cameraPosition.x += cameraMoveSpeed * ts;

    if (IsKeyHeld(GLFW_KEY_UP)) cameraPosition.y += cameraMoveSpeed * ts;
    else if (IsKeyHeld(GLFW_KEY_DOWN)) cameraPosition.y -= cameraMoveSpeed * ts;

    if (IsKeyHeld(GLFW_KEY_A)) cameraAngle += cameraRotationSpeed * ts;
    else if (IsKeyHeld(GLFW_KEY_D)) cameraAngle -= cameraRotationSpeed * ts;

    camera.SetPosition(cameraPosition);
    camera.SetRotation(cameraAngle);

    // Square Control
    if (IsKeyHeld(GLFW_KEY_J)) squarePosition.x -= squareMoveSpeed * ts;
    else if (IsKeyHeld(GLFW_KEY_L)) squarePosition.x += squareMoveSpeed * ts;

    if (IsKeyHeld(GLFW_KEY_I)) squarePosition.y += squareMoveSpeed * ts;
    else if (IsKeyHeld(GLFW_KEY_K)) squarePosition.y -= squareMoveSpeed * ts;

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), squarePosition);
    static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

    Renderer::BeginScene(camera);
    // Triangle
    // Renderer::Submit(shader, vertexArray);
    flatColorShader->Bind();
    flatColorShader->UploadUniformFloat3("u_Color", squareColor);
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 20; x++) {
            glm::vec3 deltaPos(x * 0.11f, y * 0.11f, 0.0f);
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), deltaPos + squarePosition) * scale;
            Renderer::Submit(flatColorShader, squareVA, transform);
        }
    }

    texture->Bind(diffuseTextureSlot);
    Renderer::Submit(textureShader, squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
    Renderer::EndScene();
}

void Editor::OnShutdown() {

}