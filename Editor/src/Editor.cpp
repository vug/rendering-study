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

Editor::Editor() : Application("Ugur's Editor"), squarePosition(0.0f), cameraController(*this, 1280.0f / 720.0f) { }

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

    FramebufferSpecification fbSpec;
    int w, h;
    glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);
    fbSpec.Width = w;
    fbSpec.Height = h;
    viewportFramebuffer = std::make_shared<Framebuffer>(fbSpec);
}

void Editor::OnImGuiRender() {
    if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);

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


    ImGui::Begin("Settings");
    ImGui::ColorEdit3("Square Color", glm::value_ptr(squareColor));
    std::string fps = std::string("FPS: ") + std::to_string(framesPerSecond);
    ImGui::Text(fps.c_str());
    std::string zoomLevel = std::string("Zoom Level: ") + std::to_string(cameraController.GetZoomLevel());
    ImGui::Text(zoomLevel.c_str());
    ImGui::Checkbox("Show demo window", &showDemoWindow);
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Viewport");
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    if (viewportPanelSize.x != viewportSize.x || viewportPanelSize.y != viewportSize.y) {
        viewportFramebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
        viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        cameraController.OnViewportResized(viewportPanelSize.x, viewportPanelSize.y);
    }
    uint32_t textureID = viewportFramebuffer->GetColorAttachmentRendererID();
    ImGui::Image((void*)textureID, ImVec2{ viewportSize.x, viewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End();
}

void Editor::OnUpdate(Timestep ts) {
    cameraController.OnUpdate(ts);

    viewportFramebuffer->Bind();
    RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    RenderCommand::Clear();

    // Square Control
    if (IsKeyHeld(GLFW_KEY_J)) squarePosition.x -= squareMoveSpeed * ts;
    else if (IsKeyHeld(GLFW_KEY_L)) squarePosition.x += squareMoveSpeed * ts;

    if (IsKeyHeld(GLFW_KEY_I)) squarePosition.y += squareMoveSpeed * ts;
    else if (IsKeyHeld(GLFW_KEY_K)) squarePosition.y -= squareMoveSpeed * ts;

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), squarePosition);
    static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

    Renderer::BeginScene(cameraController.GetCamera());

    auto triangleShader = shaderLibrary.Get("VertexPosColor");
    Renderer::Submit(triangleShader, triangleVA, glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));

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
    viewportFramebuffer->Unbind();
}

void Editor::OnShutdown() {

}