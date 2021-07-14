#pragma once

#include<memory>

#include "Application.h"

#include "Scene/Scene.h"
#include "Scene/SceneHierarchyPanel.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"
#include "OrthographicCameraController.h"
#include "Renderer/Texture.h"

class Editor : public Application {
public:
	Editor();
private:
	void OnInit() override;
	void OnUpdate(Timestep ts) override;
	void OnImGuiRender() override;
	void OnShutdown() override;
	void OnViewportResize(float width, float height) override;
private:
	std::shared_ptr<Scene> activeScene;
	ShaderLibrary shaderLibrary;
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
};
