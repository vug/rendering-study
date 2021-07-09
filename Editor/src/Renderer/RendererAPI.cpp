#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "RendererAPI.h"

void RendererAPI::PrintInfo() {
    std::cout << "OpenGL Info" << std::endl
        << "  Renderer: " << glGetString(GL_VENDOR) << std::endl
        << "  Vendor: " << glGetString(GL_RENDERER) << std::endl
        << "  Version: " << glGetString(GL_VERSION) << std::endl;
}

void RendererAPI::Init() {
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RendererAPI::SetClearColor(const glm::vec4& color) {
	glClearColor(color.r, color.g, color.b, color.a);
}

void RendererAPI::Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
	glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}