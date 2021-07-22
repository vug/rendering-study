#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "RenderCommand.h"
#include "Shader.h"

// Hard-coded data for Renderer to provide ready-made draw commands such as DrawFlatQuad
struct RendererData {
	std::shared_ptr<VertexArray> quadVertexArray;
	std::shared_ptr<Shader> solidColorShader;
	glm::mat4 viewProj;
};
static RendererData rendererData;

void Renderer::Init() {
	RenderCommand::Init();

	rendererData.quadVertexArray.reset(new VertexArray());
	float squareVertices[(3 + 2) * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	};

	const auto quadVertexBuffer = std::make_shared<VertexBuffer>(squareVertices, (uint32_t)sizeof(squareVertices));
	quadVertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float2, "a_TexCoord" },
	});
	rendererData.quadVertexArray->AddVertexBuffer(quadVertexBuffer);
	uint32_t squareIndices[3 * 2] = {
		0, 1, 2,
		2, 3, 0
	};
	const auto squareIB = std::make_shared<IndexBuffer>(squareIndices, (uint32_t)(sizeof(squareIndices) / sizeof(uint32_t)));
	rendererData.quadVertexArray->SetIndexBuffer(squareIB);

	rendererData.solidColorShader = std::make_shared<Shader>("assets/shaders/SolidColor.glsl");
}

void Renderer::BeginScene(const Camera& camera, const glm::mat4& cameraTransform) {
	rendererData.viewProj = camera.GetProjection() * glm::inverse(cameraTransform);
}

void Renderer::BeginScene(OrthographicCamera& camera) {
	rendererData.viewProj = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene() {

}

void Renderer::Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, GLenum primitiveType, bool isWireFrame) {
	shader->Bind();
	shader->UploadUniformMat4("u_ViewProjection", rendererData.viewProj);
	shader->UploadUniformMat4("u_Transform", transform); // ModelMatrix

	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray, 0, primitiveType, isWireFrame);
}

// High-Level Command Library

void Renderer::DrawSolidQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color) {
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1));
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));
	glm::mat4 transform = translate * rotate * scale;
	Renderer::DrawSolidQuad(transform, color);
}

void Renderer::DrawSolidQuad(const glm::mat4& transform, const glm::vec4& color) {
	rendererData.solidColorShader->Bind();
	rendererData.solidColorShader->UploadUniformFloat4("u_Color", color);
	Renderer::Submit(rendererData.solidColorShader, rendererData.quadVertexArray, transform);
}

void Renderer::DrawMesh(std::shared_ptr<VertexArray> vertexArray, const glm::mat4& transform, const glm::vec4& color) {
	rendererData.solidColorShader->Bind();
	rendererData.solidColorShader->UploadUniformFloat4("u_Color", color);
	Renderer::Submit(rendererData.solidColorShader, vertexArray, transform);
}

void Renderer::DrawLines(std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color, bool loop) {
	rendererData.solidColorShader->Bind();
	rendererData.solidColorShader->UploadUniformFloat4("u_Color", color);
	Renderer::Submit(rendererData.solidColorShader, vertexArray, transform, loop ? GL_LINE_LOOP : GL_LINE_STRIP);
}