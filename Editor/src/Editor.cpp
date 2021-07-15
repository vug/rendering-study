#include <iostream>
#include <string>

#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Editor.h"

#include "Input.h"
#include "Math.h"
#include "Scene/Components.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"

Editor::Editor() : Application("Ugur's Editor"), cameraController(1280.0f / 720.0f) { }

void Editor::OnInit() {
    RegisterScrollListener(&cameraController);
    RegisterKeyListener(this);
    //RegisterWindowListener(&cameraController);

    activeScene = std::make_shared<Scene>();
    auto square1 = activeScene->CreateEntity("Square1");
    activeScene->Reg().emplace<QuadRendererComponent>(square1, glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });
    auto redSquare = activeScene->CreateEntity("Red Square");
    activeScene->Reg().emplace<QuadRendererComponent>(redSquare, glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });
    activeScene->Reg().get<TransformComponent>(redSquare).Translation = glm::vec3{ -0.5f, 0.5f, -0.5f };
    entt::basic_handle line1{ activeScene->Reg(), activeScene->CreateEntity("Line1") };
    LineComponent& vc = line1.emplace<LineComponent>(
        std::vector<glm::vec3>{ {-0.5f, -0.5f, 0.0f}, { 0.4f, -0.4f, 0.0f }, { 0.3f, 0.3f, 0.0f }, { -0.2f, 0.2f, 0.0f } }
    );
    line1.get<TransformComponent>().Translation = { 0.0f, 0.0f, 1.0f };
    
    
    mainCameraEntity = activeScene->CreateEntity("Camera A");
    activeScene->Reg().emplace<CameraComponent>(mainCameraEntity);
    entt::basic_handle camHandle{ activeScene->Reg(), mainCameraEntity };
    camHandle.get<TransformComponent>().Translation = glm::vec3{ 0.0f, 0.0f, 5.0f };
    camHandle.get<CameraComponent>().Camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);

    secondCameraEntity = activeScene->CreateEntity("Camera B");
    activeScene->Reg().emplace<CameraComponent>(secondCameraEntity);
    activeScene->Reg().get<CameraComponent>(secondCameraEntity).Primary = false;

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

    sceneHierarchyPanel.SetContext(activeScene);
}

void Editor::OnImGuiRender() {
    static bool useCamera1 = true;
    if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);

    sceneHierarchyPanel.OnImguiRender();

    ImGui::Begin("Stats");
    std::string fps = std::string("FPS: ") + std::to_string(framesPerSecond);
    ImGui::Text(fps.c_str());
    std::string zoomLevel = std::string("Zoom Level: ") + std::to_string(cameraController.GetZoomLevel());
    ImGui::Text(zoomLevel.c_str());
    ImGui::Checkbox("Show demo window", &showDemoWindow);
    ImGui::End();
}

void Editor::OnImGuiViewportRender() {
    // Gizmos
    entt::basic_handle selectedHandle{ activeScene->Reg(), sceneHierarchyPanel.GetSelectedEntity() };
    if (selectedHandle.valid() && shouldShowGizmo) {
        ImGui::Text("Selected Entity: %s", selectedHandle.get<TagComponent>().Tag.c_str());

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

        // Camera
        auto cameraEntity = activeScene->GetPrimaryCameraEntity();
        entt::basic_handle cameraHandle{ activeScene->Reg(), cameraEntity };
        if (cameraHandle.valid()) {
            const auto& cameraComponent = cameraHandle.get<CameraComponent>();
            const glm::mat4& cameraProjection = cameraComponent.Camera.GetProjection();
            glm::mat4 cameraView = glm::inverse(cameraHandle.get<TransformComponent>().GetTransform());

            // Entity transform
            TransformComponent& tc = selectedHandle.get<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();

            // Snapping
            bool shouldSnap = Input::IsKeyHeld(GLFW_KEY_LEFT_CONTROL);
            // 45 degrees for rotation, 0.5m for translate and scale
            float snapValue = gizmoType == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;
            float snapValues[3] = { snapValue, snapValue, snapValue };

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                gizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, shouldSnap ? snapValues : nullptr);

            if (ImGuizmo::IsUsing()) {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(transform, translation, rotation, scale);

                tc.Translation = translation;
                // To prevent gimble-lock
                glm::vec3 deltaRotation = rotation - tc.Rotation;
                tc.Rotation += deltaRotation;
                tc.Scale = scale;
            }
        }
    }
}

void Editor::OnViewportResize(float width, float height) {
    cameraController.OnViewportResized(width, height);
    activeScene->OnViewportResize((uint32_t)width, (uint32_t)height);
}

void Editor::OnUpdate(Timestep ts) {
    if (GetIsViewportPaneFocused()) {
        cameraController.OnUpdate(ts);
    }

    RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    RenderCommand::Clear();

    // I J K L controls to manipulate Selected Entity's transform.
    if (GetIsViewportPaneFocused()) {
        entt::entity selectedEntity = sceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity != entt::null) {
            glm::vec3& selectedTranslation = activeScene->Reg().get<TransformComponent>(selectedEntity).Translation;
            float deltaDistance = entityMoveSpeed * ts;
            if (Input::IsKeyHeld(GLFW_KEY_J))
                selectedTranslation.x -= deltaDistance;
            else if (Input::IsKeyHeld(GLFW_KEY_L))
                selectedTranslation.x += deltaDistance;
            if (Input::IsKeyHeld(GLFW_KEY_I))
                selectedTranslation.y += deltaDistance;
            else if (Input::IsKeyHeld(GLFW_KEY_K)) 
                selectedTranslation.y -= deltaDistance;
        }
    }

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
}

void Editor::OnShutdown() {

}

void Editor::OnKeyPress(int key, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_Z:
            shouldShowGizmo = true;
            gizmoType = ImGuizmo::OPERATION::TRANSLATE;
            break;
        case GLFW_KEY_X:
            shouldShowGizmo = true;
            gizmoType = ImGuizmo::OPERATION::ROTATE;
            break;
        case GLFW_KEY_C:
            shouldShowGizmo = true;
            gizmoType = ImGuizmo::OPERATION::SCALE;
            break;
        case GLFW_KEY_V:
            shouldShowGizmo = false;
            break;
        }

    }
}