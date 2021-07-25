#pragma once

#include <filesystem>
#include<memory>

#include "Application.h"

#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include "imgui/ImGuiFileBrowser.h"

#include "Scene/Scene.h"
#include "Scene/SceneHierarchyPanel.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"
#include "OrthographicCameraController.h"
#include "Renderer/Texture.h"

class Editor : public Application, public KeyListener, public MouseButtonListener {
public:
	Editor();
private:
	void OnInit() override;
	void OnUpdate(Timestep ts) override;
	void OnImGuiRender() override;
	void OnImGuiViewportRender() override;
	void OnShutdown() override;
	void OnViewportResize(float width, float height) override;
	virtual void OnKeyPress(int key, int action, int mods) override;
	virtual void OnMouseButtonClicked(int button, int action, int mods) override;

	void NewScene();
	void OpenScene(const std::filesystem::path& fp);
	void SaveSceneAs(const std::filesystem::path& path);
private:
	std::shared_ptr<Scene> activeScene;
	OrthographicCameraController cameraController;

	std::shared_ptr<VertexArray> triangleVA;
	std::shared_ptr<Texture2D> textureCheckerboard;
	std::shared_ptr<Texture2D> textureWithAlpha;
	int diffuseTextureSlot = 0;

	entt::entity mainCameraEntity;
	entt::entity secondCameraEntity;

	float entityMoveSpeed = 5.0f;
	bool showDemoWindow = false;
	
	SceneHierarchyPanel sceneHierarchyPanel;

	ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;	
	bool shouldShowGizmo = false;

	imgui_addons::ImGuiFileBrowser file_dialog;
	bool shouldRenderOpenFileBrowser = false;
	bool shouldRenderSaveFileBrowser = false;

	int hoveredEntityID = -4;
};
