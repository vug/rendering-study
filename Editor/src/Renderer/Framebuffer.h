#pragma once

#include <stdint.h>

struct FramebufferSpecification {
	uint32_t Width, Height;
	// FramebufferFormat Format = default
	uint32_t Samples = 1;

	bool SwapChainTarget = false;
};

class Framebuffer {
public:
	Framebuffer(const FramebufferSpecification& spec);
	~Framebuffer();
	
	const FramebufferSpecification& GetSpecification() const { return specification; }
	uint32_t GetColorAttachmentRendererID() const { return colorAttachment; }

	void Invalidate();

	void Bind();
	void Unbind();
private:
	uint32_t rendererID;
	uint32_t colorAttachment, depthAttachment;
	FramebufferSpecification specification;
};
