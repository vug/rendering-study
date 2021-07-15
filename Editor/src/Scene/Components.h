#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "SceneCamera.h"
#include "../Renderer/VertexArray.h"

struct TagComponent {
	std::string Tag;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag) :
		Tag(tag) {}

};

struct TransformComponent {
	glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::vec3& translation) :
		Translation(translation) {}
	
	glm::mat4 GetTransform() const {
		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
		return glm::translate(glm::mat4(1.0f), Translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), Scale);
	}
};

struct QuadRendererComponent {
	glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

	QuadRendererComponent() = default;
	QuadRendererComponent(const QuadRendererComponent&) = default;
	QuadRendererComponent(const glm::vec4& color) :
		Color(color) {}
};

struct CameraComponent {
	SceneCamera Camera;
	bool Primary = true; // TODO: think about moving to Scene
	bool FixedAspectRatio = false;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
};

class LineComponent {
public:
	LineComponent() { ComputeVertexArray(); }
	LineComponent(const LineComponent&) = default;
	LineComponent(const std::vector<glm::vec3>& vertices) 
		: Vertices(vertices) { ComputeVertexArray(); }

	void ComputeVertexArray() {
		vertexArray.reset(new VertexArray());
		float* flat_array = static_cast<float*>(glm::value_ptr(Vertices.front()));
		const auto vertexBuffer = std::make_shared<VertexBuffer>(flat_array, (uint32_t)(sizeof(float)*3*Vertices.size()));
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },

		});
		vertexArray->AddVertexBuffer(vertexBuffer);
		std::vector<uint32_t> indices(Vertices.size());
		for (uint32_t ix = 0; ix < indices.size(); ix++) {
			indices[ix] = ix;
		}
		const auto squareIB = std::make_shared<IndexBuffer>(indices.data(), (uint32_t)(indices.size()));
		vertexArray->SetIndexBuffer(squareIB);
	}
	std::shared_ptr<VertexArray>& GetVertexArray() { return vertexArray; }
public:
	std::vector<glm::vec3> Vertices = { {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	std::shared_ptr<VertexArray> vertexArray = nullptr;
};

struct LineRendererComponent {
	glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
	bool IsLooped = false;

	LineRendererComponent() = default;
	LineRendererComponent(const LineRendererComponent&) = default;
	LineRendererComponent(const glm::vec4& color) :
		Color(color) {}
};


