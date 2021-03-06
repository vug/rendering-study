#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "RenderCommand.h"
#include "Shader.h"

// Hard-coded data for Renderer to provide ready-made draw commands such as DrawFlatQuad
struct RendererData {
	glm::mat4 viewProj;
	std::vector<Renderer::LightInfo> lightInfos;
};
static RendererData rendererData;

void Renderer::Init() {
	RenderCommand::Init();
}

void Renderer::BeginScene(const Camera& camera, const glm::mat4& cameraTransform, const std::vector<Renderer::LightInfo>& lightInfos) {
	rendererData.viewProj = camera.GetProjection() * glm::inverse(cameraTransform);
	if (lightInfos.empty()) {
		rendererData.lightInfos = { { {0, 0, 0}, 0.0f } };
	}
	else {
		rendererData.lightInfos = lightInfos;
	}
}

void Renderer::EndScene() {

}

void Renderer::Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, GLenum primitiveType, uint32_t indexOffset, uint32_t indexCount) {
	shader->Bind();
	shader->UploadUniformMat4("u_ViewProjection", rendererData.viewProj);
	shader->UploadUniformMat4("u_Transform", transform); // ModelMatrix

	std::vector<glm::vec3> lightPositions;
	std::transform(rendererData.lightInfos.begin(), rendererData.lightInfos.end(),
		std::back_inserter(lightPositions), [](LightInfo li) -> glm::vec3 { return li.position; });
	std::vector<float> lightIntensities;
	std::transform(rendererData.lightInfos.begin(), rendererData.lightInfos.end(),
		std::back_inserter(lightIntensities), [](LightInfo li) -> float { return li.intensity; });

	shader->UploadUniformFloat3s("u_LightPositions", lightPositions);
	shader->UploadUniformFloats("u_LightIntensities", lightIntensities);
	shader->UploadUniformInt("u_LightCount", rendererData.lightInfos.size());

	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray, indexCount, primitiveType, indexOffset);
}

// High-Level Command Library

void Renderer::DrawMesh(MeshComponent& mesh, MeshRendererComponent& meshRenderer, std::shared_ptr<Shader> shader, TransformComponent& transform) {
	shader->Bind();
	shader->UploadUniformFloat4("u_Color", meshRenderer.Color);
	Renderer::Submit(shader, mesh.vertexArray, transform.GetTransform());
}

void Renderer::DrawMeshTriangle(const MeshComponent& mesh, const MeshRendererComponent& meshRenderer, const std::shared_ptr<Shader>& shader, const TransformComponent& transform, uint32_t triangleNo) {
	shader->Bind();
	shader->UploadUniformFloat4("u_Color", meshRenderer.Color);
	Renderer::Submit(shader, mesh.vertexArray, transform.GetTransform(), GL_TRIANGLES, triangleNo * 3, 3);
}

void Renderer::DrawLines(std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color, bool loop) {
	std::shared_ptr<Shader> shader = ShaderLibrary::Instance().Get("SolidColor");
	shader->Bind();
	shader->UploadUniformFloat4("u_Color", color);
	Renderer::Submit(shader, vertexArray, transform, loop ? GL_LINE_LOOP : GL_LINE_STRIP);
}