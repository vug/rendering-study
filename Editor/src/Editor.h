#pragma once

#include <filesystem>
#include<memory>

#include "Application.h"

#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include "imgui/ImGuiFileBrowser.h"

#include "Layers/Layer.h"
#include "Scene/Scene.h"
#include "Scene/SceneHierarchyPanel.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"

class Editor : public Application, public KeyListener, public MouseButtonListener {
public:
	Editor();
	void AddLayer(Layer* layer);
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

	EditorCamera editorCamera;


	float entityMoveSpeed = 5.0f;

	bool showDemoWindow = false;
	
	SceneHierarchyPanel sceneHierarchyPanel;

	ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;	
	bool shouldShowGizmo = false;

	imgui_addons::ImGuiFileBrowser file_dialog;
	bool shouldRenderOpenFileBrowser = false;
	bool shouldRenderSaveFileBrowser = false;

	int hoveredEntityID = -4;

	std::vector<Layer*> layers;
};
