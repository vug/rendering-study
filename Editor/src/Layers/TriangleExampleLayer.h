#pragma once

#include <memory>

#include "Layer.h"
#include "../Renderer/VertexArray.h"
#include "../Renderer/Texture.h"

class TriangleExampleLayer : public Layer {
public:
	virtual void OnInit() override;
	virtual void OnUpdate(Timestep ts) override;
private:
	std::shared_ptr<VertexArray> triangleVA;
	std::shared_ptr<VertexArray> quadVertexArray;
	std::shared_ptr<Texture2D> textureCheckerboard;
	std::shared_ptr<Texture2D> textureWithAlpha;
	int diffuseTextureSlot = 0;
};
