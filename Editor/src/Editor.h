#pragma once

#include<memory>

#include "Application.h"

#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"

class Editor : public Application {
public:
	Editor();
private:
	void OnInit();
	void OnUpdate();
	void OnImGuiRender();
	void OnShutdown();
private:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<VertexArray> vertexArray;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;

	std::shared_ptr<Shader> blueShader;
	std::shared_ptr<VertexArray> squareVA;
	std::shared_ptr<VertexBuffer> squareVB;
	std::shared_ptr<IndexBuffer> squareIB;

	OrthographicCamera camera = OrthographicCamera(-1.6f, 1.6f, -0.9f, 0.9f);
    float cameraMoveSpeed = 0.1f;
    float cameraRotationSpeed = 2.0f;
	bool showDemoWindow = false;
};
