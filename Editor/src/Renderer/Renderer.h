#pragma once

#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"

class Renderer {
public:
	static void BeginScene(OrthographicCamera& camera);
	static void EndScene();

	static void Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray);
private:
	struct SceneData {
		glm::mat4 viewProjectionMatrix;
	};

	static SceneData* sceneData;
};
