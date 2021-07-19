#pragma once

#include <cassert>
#include <stdint.h>
#include <vector>

enum class FramebufferTextureFormat
{
	None = 0,

	// Color
	RGBA8,
	// RBGA16F will come here

	// Depth/stencil
	DEPTH24STENCIL8,

	// Defaults
	Depth = DEPTH24STENCIL8
};

struct FramebufferTextureSpecification {
	// Format, Wrap, Filtering
	FramebufferTextureSpecification() = default;
	FramebufferTextureSpecification(FramebufferTextureFormat format)
		: TextureFormat(format) {}

	FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
	// TODO: filtering/wrap
};

struct FramebufferAttachmentSpecification {
	FramebufferAttachmentSpecification() = default;
	FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
		: Attachments(attachments) {}

	std::vector<FramebufferTextureSpecification> Attachments;
};

struct FramebufferSpecification {
	uint32_t Width, Height;
	FramebufferAttachmentSpecification Attachments;
	uint32_t Samples = 1;

	bool SwapChainTarget = false;
};

class Framebuffer {
public:
	Framebuffer(const FramebufferSpecification& spec);
	~Framebuffer();
	
	const FramebufferSpecification& GetSpecification() const { return specification; }
	uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const { 
		assert(index < colorAttachmentRendererIDs.size()); 
		return colorAttachmentRendererIDs[index]; 
	}

	void Invalidate();

	void Bind();
	void Unbind();

	void Resize(uint32_t width, uint32_t height);
private:
	uint32_t rendererID = 0;
	FramebufferSpecification specification;

	std::vector<FramebufferTextureSpecification> colorAttachmentSpecs;
	FramebufferTextureSpecification depthAttachmentSpec = FramebufferTextureFormat::None;

	std::vector<uint32_t> colorAttachmentRendererIDs;
	uint32_t depthAttachmentRendererID = 0;
};
