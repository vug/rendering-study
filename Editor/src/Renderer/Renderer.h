#pragma once

#include "RenderCommand.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "../Scene/Components.h"

class Renderer {
public:
	static void Init();

	static void BeginScene(const Camera& camera, const glm::mat4& cameraTransform);
	static void EndScene();

	static void Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f), GLenum primitiveType = GL_TRIANGLES, uint32_t indexOffset = 0, uint32_t indexCount = 0);

	static void DrawMesh(MeshComponent& mesh, MeshRendererComponent& meshRenderer, std::shared_ptr<Shader> shader, TransformComponent& transform);
	static void DrawMeshTriangle(const MeshComponent& mesh, const MeshRendererComponent& meshRenderer, const std::shared_ptr<Shader>& shader, const TransformComponent& transform, uint32_t triangleNo);
	static void DrawLines(std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color, bool loop = false);
};
