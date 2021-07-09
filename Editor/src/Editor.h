#pragma once

#include<memory>

#include "Application.h"

#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"
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
	std::shared_ptr<Shader> shader;
	std::shared_ptr<VertexArray> vertexArray;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;

	std::shared_ptr<Shader> flatColorShader, textureShader;
	std::shared_ptr<VertexArray> squareVA;
	std::shared_ptr<VertexBuffer> squareVB;
	std::shared_ptr<IndexBuffer> squareIB;

	std::shared_ptr<Texture2D> texture;
	int diffuseTextureSlot = 0;

	OrthographicCamera camera = OrthographicCamera(-1.6f, 1.6f, -0.9f, 0.9f);
	glm::vec3 cameraPosition = { 0.0f, 0.0f, 0.0f };
    float cameraMoveSpeed = 5.0f;
	float cameraAngle = 0.0f;
	float cameraRotationSpeed = 180.0f;
	glm::vec3 squarePosition;
	float squareMoveSpeed = 1.0f;
	glm::vec3 squareColor = { 0.2f, 03.f, 0.8f };
	bool showDemoWindow = false;
};
