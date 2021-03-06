#include "RenderCommand.h"

#include <iostream>

#include <GLFW/glfw3.h>
#include <stb/stb_image_write.h>

FrameStats RenderCommand::frameStats = {};
bool RenderCommand::shouldDebugRenderSingleFrame = false;
static int drawCallNo;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

void RenderCommand::Init(bool isWireframe, bool onlyFrontFaces) {
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

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
	drawCallNo = 1;
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

	// Collect Frame Render Stats
	frameStats.drawCalls += 1;
	if (primitiveType == GL_TRIANGLES) {
		frameStats.triangles += count / 3;
	}
	if (primitiveType == GL_LINE_LOOP || primitiveType == GL_LINE_STRIP) {
		frameStats.lines += count;
	}

	// Save Framebuffer to a file after each draw call 
	if (shouldDebugRenderSingleFrame) {
		// Bound Framebuffer Size
		GLint dims[4] = { 0 };
		glGetIntegerv(GL_VIEWPORT, dims);
		GLint width = dims[2];
		GLint height = dims[3];
		
		// Read bound framebuffer pixel data into the buffer
		GLsizei nrChannels = 4;
		GLsizei stride = nrChannels * width;
		stride += (stride % 4) ? (4 - stride % 4) : 0;
		GLsizei bufferSize = stride * height;
		std::vector<unsigned char> buffer(bufferSize);
		glPixelStorei(GL_PACK_ALIGNMENT, 4);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

		// Save buffer into a file
		stbi_flip_vertically_on_write(true);
		std::string filename = std::string("call-") + std::to_string(drawCallNo) + ".png";
		stbi_write_png(filename.c_str(), width, height, nrChannels, buffer.data(), stride);

		drawCallNo++;
	}
}

void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	glViewport(x, y, width, height);
}