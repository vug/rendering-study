#include "Framebuffer.h"

#include <cassert>

#include <glad/glad.h>

namespace {
	static GLenum TextureTarget(bool multisampled) {
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count) {
		glCreateTextures(TextureTarget(multisampled), count, outID);
	}

	static void BindTexture(bool multisampled, uint32_t id) {
		glBindTexture(TextureTarget(multisampled), id);
	}

	static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index) {
		bool multisampled = samples > 1;
		if (multisampled) {
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

			// Will be set by framebuffer specification in the future
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
	}

	static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height) {
		bool multisampled = samples > 1;
		if (multisampled) {
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		}
		else {
			glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

			// Will be set by framebuffer specification in the future
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
	}

	static bool IsDepthFormat(FramebufferTextureFormat format) {
		switch (format) {
		case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
		}
		return false;
	}
}

Framebuffer::Framebuffer(const FramebufferSpecification& spec)
	: specification(spec) {
	for (auto spec : specification.Attachments.Attachments) {
		if (!IsDepthFormat(spec.TextureFormat))
			colorAttachmentSpecs.emplace_back(spec);
		else
			depthAttachmentSpec = spec;
	}
	Invalidate();
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &rendererID);
	glDeleteTextures((GLsizei)colorAttachmentRendererIDs.size(), colorAttachmentRendererIDs.data());
	glDeleteTextures(1, &depthAttachmentRendererID);
}

void Framebuffer::Invalidate() {
	if (rendererID) {
		glDeleteFramebuffers(1, &rendererID);
		glDeleteTextures((GLsizei)colorAttachmentRendererIDs.size(), colorAttachmentRendererIDs.data());
		glDeleteTextures(1, &depthAttachmentRendererID);

		colorAttachmentRendererIDs.clear();
		depthAttachmentRendererID = 0;
	}

	glCreateFramebuffers(1, &rendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

	bool isMultisample = specification.Samples > 1;
	// Attachments
	if (colorAttachmentSpecs.size()) {
		colorAttachmentRendererIDs.resize(colorAttachmentSpecs.size());
		CreateTextures(isMultisample, colorAttachmentRendererIDs.data(), (uint32_t)colorAttachmentRendererIDs.size());
		for (size_t i = 0; i < colorAttachmentRendererIDs.size(); i++) {
			BindTexture(isMultisample, colorAttachmentRendererIDs[i]);
			switch (colorAttachmentSpecs[i].TextureFormat) {
			case FramebufferTextureFormat::RGBA8:
				AttachColorTexture(colorAttachmentRendererIDs[i], specification.Samples, GL_RGBA8, GL_RGBA, specification.Width, specification.Height, (int)i);
				break;
			case FramebufferTextureFormat::RED_INTEGER:
				AttachColorTexture(colorAttachmentRendererIDs[i], specification.Samples, GL_R32I, GL_RED_INTEGER, specification.Width, specification.Height, (int)i);
				break;
			}
		}
	}

	if (depthAttachmentSpec.TextureFormat != FramebufferTextureFormat::None) {
		CreateTextures(isMultisample, &depthAttachmentRendererID, 1);
		BindTexture(isMultisample, depthAttachmentRendererID);
		switch (depthAttachmentSpec.TextureFormat) {
		case FramebufferTextureFormat::DEPTH24STENCIL8:
			AttachDepthTexture(depthAttachmentRendererID, specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, specification.Width, specification.Height);
		}
	}

	if (colorAttachmentRendererIDs.size() > 1) {
		assert(colorAttachmentRendererIDs.size() <= 4); // only support up to 4
		GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers((GLsizei)colorAttachmentRendererIDs.size(), buffers); // enable them for rendering
	}
	else if (colorAttachmentRendererIDs.empty()) {
		glDrawBuffer(GL_NONE);
	}

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

int Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) {
	assert(attachmentIndex < colorAttachmentRendererIDs.size());
	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
	int pixelData;
	glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
	return pixelData;
}

