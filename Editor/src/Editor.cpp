#include <iostream>
#include <string>

#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Editor.h"

#include "Input.h"
#include "Scene/Components.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"

Editor::Editor() : Application("Ugur's Editor"), cameraController(1280.0f / 720.0f) { }

void Editor::OnInit() {
    RegisterScrollListener(&cameraController);
    //RegisterWindowListener(&cameraController);

    activeScene = std::make_shared<Scene>();
    auto square1 = activeScene->CreateEntity();
    selectedObject = square1;
    activeScene->Reg().emplace<TransformComponent>(square1);
    activeScene->Reg().emplace<QuadRendererComponent>(square1, glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

    shaderLibrary.Load("assets/shaders/VertexPosColor.glsl");
    //auto textureShader = shaderLibrary.Load("assets/shaders/Texture.glsl");
    //textureShader->Bind();
    //textureShader->UploadUniformInt("u_Texture", diffuseTextureSlot);
    //textureCheckerboard.reset(new Texture2D("assets/textures/Checkerboard.png"));
    //textureWithAlpha.reset(new Texture2D("assets/textures/ChernoLogo.png"));
       
    triangleVA.reset(new VertexArray());  
    float triangleVertices[3 * 7] = {
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
    };
    const auto triangleVB = std::make_shared<VertexBuffer>(triangleVertices, (uint32_t)sizeof(triangleVertices));
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" },
    };
    triangleVB->SetLayout(layout);
    triangleVA->AddVertexBuffer(triangleVB);

    uint32_t triangleIndices[3] = { 0, 1, 2 };
    const auto triangleIB = std::make_shared<IndexBuffer>(triangleIndices, (uint32_t)(sizeof(triangleIndices) / sizeof(uint32_t)));
    triangleVA->SetIndexBuffer(triangleIB);
}

void Editor::OnImGuiRender() {
    if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);

    ImGui::Begin("Settings");
    auto& selectedQuad = activeScene->Reg().get<QuadRendererComponent>(selectedObject);
    ImGui::ColorEdit3("Square Color", glm::value_ptr(selectedQuad.Color));
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
    activeScene->OnUpdate(ts);
    // Non-Scene example rendering commands. Will be removed or moved into a Scene
    {
        auto triangleShader = shaderLibrary.Get("VertexPosColor");
        Renderer::Submit(triangleShader, triangleVA, glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));

        //auto textureShader = shaderLibrary.Get("Texture");
        //textureCheckerboard->Bind(diffuseTextureSlot);
        //Renderer::Submit(textureShader, squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        //textureWithAlpha->Bind(diffuseTextureSlot);
        //Renderer::Submit(textureShader, squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.1f)));
    }

    Renderer::EndScene();
}

void Editor::OnShutdown() {

}