#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "VertexArray.h"

class RendererAPI {
public:
	static void PrintInfo();
	void Init();

	void SetClearColor(const glm::vec4& color);
	void Clear();

	void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray);
};