#include "RenderCommand.h"

#include <iostream>

#include <GLFW/glfw3.h>

void RenderCommand::Init() {
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
}

void RenderCommand::PrintInfo() {
	std::cout << "OpenGL Info" << std::endl
		<< "  Renderer: " << glGetString(GL_VENDOR) << std::endl
		<< "  Vendor: " << glGetString(GL_RENDERER) << std::endl
		<< "  Version: " << glGetString(GL_VERSION) << std::endl;
}

void RenderCommand::SetClearColor(const glm::vec4& color) {
	glClearColor(color.r, color.g, color.b, color.a);
}

void RenderCommand::Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderCommand::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount, GLenum primitiveType, bool isWireFrame) {
	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	if (isWireFrame) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	glDrawElements(primitiveType, count, GL_UNSIGNED_INT, nullptr);
	if (isWireFrame) {
		glPolygonMode(GL_FRONT, GL_FILL);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	glViewport(x, y, width, height);
}