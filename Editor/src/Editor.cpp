#include <iostream>
#include <string>

#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Editor.h"

#include "Input.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"

Editor::Editor() : Application("Ugur's Editor"), squarePosition(0.0f), cameraController(1280.0f / 720.0f) { }

void Editor::OnInit() {
    RegisterScrollListener(&cameraController);
    //RegisterWindowListener(&cameraController);

    shaderLibrary.Load("assets/shaders/VertexPosColor.glsl");
    shaderLibrary.Load("assets/shaders/FlatColor.glsl");
    auto textureShader = shaderLibrary.Load("assets/shaders/Texture.glsl");
    textureShader->Bind();
    textureShader->UploadUniformInt("u_Texture", diffuseTextureSlot);
    textureCheckerboard.reset(new Texture2D("assets/textures/Checkerboard.png"));
    textureWithAlpha.reset(new Texture2D("assets/textures/ChernoLogo.png"));
       
    triangleVA.reset(new VertexArray());  
    float vertices[3 * 7] = {
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
    };
    const auto vertexBuffer = std::make_shared<VertexBuffer>(vertices, (uint32_t)sizeof(vertices));
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" },
    };
    vertexBuffer->SetLayout(layout);
    triangleVA->AddVertexBuffer(vertexBuffer);

    uint32_t indices[3] = { 0, 1, 2 };
    const auto indexBuffer = std::make_shared<IndexBuffer>(indices, (uint32_t)(sizeof(indices) / sizeof(uint32_t)));
    triangleVA->SetIndexBuffer(indexBuffer);

    squareVA.reset(new VertexArray());
    float squareVertices[(3 + 2) * 4] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
    };
    const auto squareVB = std::make_shared<VertexBuffer>(squareVertices, (uint32_t)sizeof(squareVertices));
    squareVB->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float2, "a_TexCoord" },
        });
    squareVA->AddVertexBuffer(squareVB);
    uint32_t squareIndices[3 * 2] = {
        0, 1, 2,
        2, 3, 0
    };    
    const auto squareIB = std::make_shared<IndexBuffer>(squareIndices, (uint32_t)(sizeof(squareIndices) / sizeof(uint32_t)));
    squareVA->SetIndexBuffer(squareIB);
}

void Editor::OnImGuiRender() {
    if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);

    ImGui::Begin("Settings");
    ImGui::ColorEdit3("Square Color", glm::value_ptr(squareColor));
    std::string fps = std::string("FPS: ") + std::to_string(framesPerSecond);
    ImGui::Text(fps.c_str());
    std::string zoomLevel = std::string("Zoom Level: ") + std::to_string(cameraController.GetZoomLevel());
    ImGui::Text(zoomLevel.c_str());
    ImGui::Checkbox("Show demo window", &showDemoWindow);
    ImGui::End();
}

void Editor::OnViewportResize(float width, float height) {
    cameraController.OnViewportResized(width, height);
}

void Editor::OnUpdate(Timestep ts) {
    if (GetIsViewportPaneFocused()) {
        cameraController.OnUpdate(ts);
    }
    RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    RenderCommand::Clear();

    // Square Control
    if (GetIsViewportPaneFocused()) {
        if (Input::IsKeyHeld(GLFW_KEY_J)) squarePosition.x -= squareMoveSpeed * ts;
        else if (Input::IsKeyHeld(GLFW_KEY_L)) squarePosition.x += squareMoveSpeed * ts;
        if (Input::IsKeyHeld(GLFW_KEY_I)) squarePosition.y += squareMoveSpeed * ts;
        else if (Input::IsKeyHeld(GLFW_KEY_K)) squarePosition.y -= squareMoveSpeed * ts;
    }

    Renderer::BeginScene(cameraController.GetCamera());

    auto triangleShader = shaderLibrary.Get("VertexPosColor");
    Renderer::Submit(triangleShader, triangleVA, glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));

    static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    auto flatColorShader = shaderLibrary.Get("FlatColor");
    flatColorShader->Bind();
    flatColorShader->UploadUniformFloat3("u_Color", squareColor);
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 20; x++) {
            glm::vec3 deltaPos(x * 0.11f, y * 0.11f, 0.0f);
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), deltaPos + squarePosition) * scale;
            Renderer::Submit(flatColorShader, squareVA, transform);
        }
    }

    auto textureShader = shaderLibrary.Get("Texture");
    textureCheckerboard->Bind(diffuseTextureSlot);
    Renderer::Submit(textureShader, squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

    textureWithAlpha->Bind(diffuseTextureSlot);
    Renderer::Submit(textureShader, squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.1f)));

    Renderer::EndScene();
}

void Editor::OnShutdown() {

}