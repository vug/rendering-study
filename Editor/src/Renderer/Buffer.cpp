#include "Buffer.h"

#include "glad/glad.h"

/**
 * Vertex Buffer
 */

VertexBuffer::VertexBuffer(float* vertices, uint32_t size) {
    glCreateBuffers(1, &rendererID);
    Update(vertices, size);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &rendererID);
}

void VertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}

void VertexBuffer::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Update(float* vertices, uint32_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

/**
 * Index Buffer
 */

IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count)
    : count(count) {
    glCreateBuffers(1, &rendererID);
    Update(indices, count);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &rendererID);
}

void IndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
}

void IndexBuffer::Unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Update(uint32_t* indices, uint32_t count) {
    // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
    // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    this->count = count;
}
