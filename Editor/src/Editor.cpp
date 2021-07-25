#include <iostream>
#include <string>

#include "entt/entt.hpp"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include "imgui/ImGuiFileBrowser.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Editor.h"

#include "Input.h"
#include "Layers/TriangleExampleLayer.h"
#include "Math.h"
#include "Scene/Components.h"
#include "Scene/SceneSerializer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"

Editor::Editor() : Application("Ugur's Editor"), 
    activeScene(std::make_shared<Scene>()), 
    editorCamera(EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f)) {

    //AddLayer(new TriangleExampleLayer());
}

void Editor::AddLayer(Layer* layer) {
    layers.push_back(layer);
}

void Editor::OnInit() {
    RegisterScrollListener(&editorCamera);
    RegisterKeyListener(this);
    RegisterMouseButtonListener(this);

    ShaderLibrary::Instance().Load("assets/shaders/SolidColor.glsl");
    ShaderLibrary::Instance().Load("assets/shaders/FlatShader.glsl");
       
    sceneHierarchyPanel.SetContext(activeScene);

    for (auto& layer : layers)
        layer->OnInit();
}

void Editor::OnImGuiRender() {
    if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                NewScene();
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                shouldRenderOpenFileBrowser = true;
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                shouldRenderSaveFileBrowser = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Ctrl+Q")) {
                Application::Close();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();

        if (shouldRenderOpenFileBrowser)
            ImGui::OpenPopup("Open File");
        if (shouldRenderSaveFileBrowser)
            ImGui::OpenPopup("Save File");


        if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".scene")) {
            OpenScene(file_dialog.selected_path);
        }
        if (file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), ".scene")) {
            std::cout << "save?" << std::endl;
            SaveSceneAs(file_dialog.selected_path);
        }
        shouldRenderOpenFileBrowser = false;
        shouldRenderSaveFileBrowser = false;
    }

    sceneHierarchyPanel.OnImguiRender();

    ImGui::Begin("Stats");

    std::string name = "None";
    if (hoveredEntityID >= 0) {
        entt::basic_handle<entt::entity> hoveredEntityHandle = { activeScene->Reg(), (entt::entity)hoveredEntityID };
        if (hoveredEntityHandle.valid())
            name = hoveredEntityHandle.get<TagComponent>().Tag;
    }
    ImGui::Text("Hovered Entity: %s", name.c_str());

    ImGui::Separator();
    ImGui::Text("Editor Camera");
    glm::vec3 pos = editorCamera.GetPosition();
    ImGui::Text("Position: %.1f, %.1f, %.1f", pos.x, pos.y, pos.z);
    glm::vec3 foc = editorCamera.GetFocalPoint();
    ImGui::Text("Focal Pt: %.1f, %.1f, %.1f", foc.x, foc.y, foc.z);
    ImGui::Text("Distance: %.1f", editorCamera.GetDistance());
    ImGui::Text("PitchYaw: %.1f, %1.f", editorCamera.GetPitch(), editorCamera.GetYaw());

    ImGui::Separator();
    ImGui::Text("FPS: %.1f", framesPerSecond);

    ImGui::Separator();
    ImGui::Checkbox("Wireframe", &activeScene->renderWireframe);
    ImGui::Checkbox("Only Front Faces", &activeScene->renderOnlyFront);

    ImGui::Separator();
    if (ImGui::Button("Solid Color", ImVec2{ 100.0, 25.0 }))
        activeScene->renderFlatShading = false;
    if (ImGui::Button("Flat Shading", ImVec2{ 100.0, 25.0 }))
        activeScene->renderFlatShading = true;

    ImGui::Separator();
    ImGui::Checkbox("Show demo window", &showDemoWindow);

    ImGui::End();
}

void Editor::OnImGuiViewportRender() {
    // Transformation Gizmos
    entt::basic_handle selectedHandle{ activeScene->Reg(), sceneHierarchyPanel.GetSelectedEntity() };
    if (selectedHandle.valid() && shouldShowGizmo) {
        ImGui::Text("Selected Entity: %s", selectedHandle.get<TagComponent>().Tag.c_str());

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(viewportBounds[0].x, viewportBounds[0].y, viewportBounds[1].x - viewportBounds[0].x, viewportBounds[1].y - viewportBounds[0].y);

        // 
        glm::mat4 cameraProjection;
        glm::mat4 cameraView;
        // Scene Camera
        auto sceneCameraEntity = activeScene->GetPrimaryCameraEntity();
        entt::basic_handle sceneCameraHandle{ activeScene->Reg(), sceneCameraEntity };
        if (sceneCameraHandle.valid()) {
            const auto& cameraComponent = sceneCameraHandle.get<CameraComponent>();
            cameraProjection = cameraComponent.Camera.GetProjection();
            cameraView = glm::inverse(sceneCameraHandle.get<TransformComponent>().GetTransform());
        }
        else { // Editor Camera always exists
            cameraProjection = editorCamera.GetProjection();
            cameraView = editorCamera.GetViewMatrix();
            cameraView = glm::inverse(cameraView);
        }

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

void Editor::OnViewportResize(float width, float height) {
    activeScene->OnViewportResize((uint32_t)width, (uint32_t)height);
    editorCamera.SetViewportSize(width, height);
}

void Editor::OnUpdate(Timestep ts) {
    editorCamera.OnUpdate(ts);

    RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    RenderCommand::Clear();

    constexpr int entityIdAttachmentIndex = 1;
    viewportFramebuffer->ClearAttachment(entityIdAttachmentIndex, -1);

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

    activeScene->OnUpdate(ts, editorCamera);

    auto [mx, my] = ImGui::GetMousePos();
    mx -= viewportBounds[0].x;
    my -= viewportBounds[0].y;
    glm::vec2 viewportSize = viewportBounds[1] - viewportBounds[0];
    my = viewportSize.y - my;

    int mouseX = (int)mx;
    int mouseY = (int)my;

    if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
        hoveredEntityID = viewportFramebuffer->ReadPixel(1, mouseX, mouseY);
    }

    for (auto& layer : layers)
        layer->OnUpdate(ts);
}

void Editor::OnShutdown() {

}

void Editor::OnKeyPress(int key, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        // Transform Gizmos
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
        // File Dialogs
        case GLFW_KEY_N:
            if (mods & GLFW_MOD_CONTROL)
                NewScene();
            break;
        case GLFW_KEY_O:
            if (mods & GLFW_MOD_CONTROL)
                shouldRenderOpenFileBrowser = true;
            break;
        case GLFW_KEY_S:
            if (mods & (GLFW_MOD_SHIFT | GLFW_MOD_CONTROL))
                shouldRenderOpenFileBrowser = true;
            break;
        case GLFW_KEY_Q:
            if (mods & GLFW_MOD_CONTROL)
                Application::Close();
        }
    }
}

void Editor::OnMouseButtonClicked(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGuizmo::IsOver()) {
        sceneHierarchyPanel.SetSelectedEntity((entt::entity)hoveredEntityID);
    }
}

void Editor::NewScene() {
    activeScene = std::make_shared<Scene>();
    //activeScene->OnViewportResize();
    sceneHierarchyPanel.SetContext(activeScene);
}

void Editor::OpenScene(const std::filesystem::path& path) {
    SceneSerializer serializer(activeScene);
    serializer.Deserialize(path);
}

void Editor::SaveSceneAs(const std::filesystem::path& path) {
    SceneSerializer serializer(activeScene);
    serializer.Serialize(path);
}
