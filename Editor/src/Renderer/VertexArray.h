#pragma once

#include <memory>
#include <vector>

#include "Buffer.h"

class VertexArray {
public:
	VertexArray();
	~VertexArray();

	void Bind() const;
	void Unbind() const;

	void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

	const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() { return vertexBuffers; }
	const std::shared_ptr<IndexBuffer>& GetIndexBuffer() { return indexBuffer; }
private:
	uint32_t rendererID;
	std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
	std::shared_ptr<IndexBuffer> indexBuffer;
};
