#pragma once
#include <memory>

#include <glm/glm.hpp>

#include "VertexArray.h"

class RenderCommand {
public:
	static void Init();
	static void PrintInfo();
	static void SetClearColor(const glm::vec4& color);
	static void Clear();
	static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0);
	static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
};
