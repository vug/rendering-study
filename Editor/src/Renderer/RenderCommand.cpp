#include "RenderCommand.h"

#include <iostream>

#include <GLFW/glfw3.h>

FrameStats RenderCommand::frameStats = {};

void RenderCommand::Init(bool isWireframe, bool onlyFrontFaces) {
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	//glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	if (isWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_LINE_SMOOTH); // anti-aliasing
		glLineWidth(2.0f);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (onlyFrontFaces) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	frameStats = { 0, 0, 0 };
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
	glDepthMask(GL_TRUE); // when not set true depth buffer won't be cleaned
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderCommand::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount, GLenum primitiveType, uint32_t indexOffset) {
	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	glDrawElements(primitiveType, count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * indexOffset));
	glBindTexture(GL_TEXTURE_2D, 0);

	frameStats.drawCalls += 1;
	if (primitiveType == GL_TRIANGLES) {
		frameStats.triangles += count / 3;
	}
	if (primitiveType == GL_LINE_LOOP || primitiveType == GL_LINE_STRIP) {
		frameStats.lines += count;
	}
}

void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	glViewport(x, y, width, height);
}