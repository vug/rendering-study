#include "Framebuffer.h"

#include <cassert>

#include <glad/glad.h>

Framebuffer::Framebuffer(const FramebufferSpecification& spec)
	: specification(spec) {
	Invalidate();
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &rendererID);
	glDeleteTextures(1, &colorAttachment);
	glDeleteTextures(1, &depthAttachment);
}

void Framebuffer::Invalidate() {
	if (rendererID) {
		glDeleteFramebuffers(1, &rendererID);
		glDeleteTextures(1, &colorAttachment);
		glDeleteTextures(1, &depthAttachment);
	}

	glCreateFramebuffers(1, &rendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

	glCreateTextures(GL_TEXTURE_2D, 1, &colorAttachment);
	glBindTexture(GL_TEXTURE_2D, colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specification.Width, specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &depthAttachment);
	glBindTexture(GL_TEXTURE_2D, depthAttachment);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, specification.Width, specification.Height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE); // Framebuffer incomplete!

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
	glViewport(0, 0, specification.Width, specification.Height);
}

void Framebuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(uint32_t width, uint32_t height) {
	specification.Width = width;
	specification.Height = height;
	Invalidate();
}

