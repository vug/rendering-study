#pragma once

#include "RenderCommand.h"
#include "Camera.h"
#include "OrthographicCamera.h"
#include "Shader.h"
#include "Texture.h"

class Renderer {
public:
	static void Init();

	static void BeginScene(OrthographicCamera& camera); // TODO: remove
	static void BeginScene(const Camera& camera, const glm::mat4& cameraTransform);
	static void EndScene();

	static void DrawSolidQuad(const glm::vec3& position = { 0.0f, 0.0f, 0.0f }, float rotation = 0.0f, const glm::vec2& size = { 1.0f, 1.0f }, const glm::vec4& color = { 1.0f, 0.0f, 1.0f, 1.0f });
	static void DrawSolidQuad(const glm::mat4& transform, const glm::vec4& color = { 1.0f, 0.0f, 1.0f, 1.0f });
	//static void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const Texture2D& texture);
	static void DrawMesh(std::shared_ptr<VertexArray> vertexArray, const glm::mat4& transform, const glm::vec4& color);
	static void DrawLines(std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color, bool loop = false);

	static void Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f), GLenum primitiveType = GL_TRIANGLES);
};
