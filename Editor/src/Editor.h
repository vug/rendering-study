#pragma once

#include<memory>

#include "Application.h"

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
private:
	ShaderLibrary shaderLibrary;
	std::shared_ptr<VertexArray> vertexArray;
	std::shared_ptr<VertexArray> squareVA;
	std::shared_ptr<Texture2D> texture;
	std::shared_ptr<Texture2D> textureWithAlpha;
	int diffuseTextureSlot = 0;

	OrthographicCameraController cameraController;

	glm::vec3 squarePosition;
	float squareMoveSpeed = 1.0f;
	glm::vec3 squareColor = { 0.2f, 03.f, 0.8f };
	bool showDemoWindow = false;
};
