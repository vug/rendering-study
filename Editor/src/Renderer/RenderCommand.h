#pragma once
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "VertexArray.h"

struct FrameStats {
	int drawCalls = 0;
	int triangles = 0;
	int lines = 0;
};

class RenderCommand {
public:
	static void Init(bool isWireframe = false, bool onlyFrontFaces = false);
	static void PrintInfo();
	static void SetClearColor(const glm::vec4& color);
	static void Clear();
	static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0, GLenum primitiveType = GL_TRIANGLES, uint32_t indexOffset = 0);
	static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	static const FrameStats& GetStats() { return frameStats; }
public:
	static FrameStats frameStats;
	static bool shouldDebugRenderSingleFrame;
};
