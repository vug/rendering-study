#pragma once

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "SceneCamera.h"
#include "../Renderer/VertexArray.h"

class Component {
public:
	static const inline char* GetName() = delete;
};

struct TagComponent : public Component {
	static const inline char* GetName() { return "TagComponent"; }

	std::string Tag;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag) :
		Tag(tag) {}

};

struct TransformComponent : public Component {
	static const inline char* GetName() { return "TransformComponent"; }

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

struct CameraComponent : public Component {
	static const inline char* GetName() { return "CameraComponent"; }

	SceneCamera Camera;
	bool Primary = true; // TODO: think about moving to Scene
	bool FixedAspectRatio = false;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
};

class LineComponent : public Component {
public:
	static const inline char* GetName() { return "LineComponent"; }

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
			//{ ShaderDataType::Int, "a_EntityID" },
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

class MeshComponent : public Component{
public:
	static const inline char* GetName() { return "MeshComponent"; }

	struct MeshVertex {
		glm::vec3 Position;
		int EntityID = -2; // -2 when MeshVertex was not given an entityID
	};

	MeshComponent() {
		const auto vertexBuffer = std::make_shared<VertexBuffer>();
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Int, "a_EntityID" },
		});
		vertexBuffer->Update(Vertices.data(), (uint32_t)(sizeof(MeshVertex) * 3 * Vertices.size()));

		uint32_t* flat_index_array = static_cast<uint32_t*>(glm::value_ptr(Indices.front()));
		const auto squareIB = std::make_shared<IndexBuffer>(flat_index_array, (uint32_t)(3 * Indices.size()));

		vertexArray = std::make_shared<VertexArray>();
		vertexArray->AddVertexBuffer(vertexBuffer);
		vertexArray->SetIndexBuffer(squareIB);
	}
	MeshComponent(const MeshComponent&) = default;

	void ComputeVertexArray() {
		for (auto& v : Vertices) { v.EntityID = entityID; }
		vertexArray->GetVertexBuffers()[0]->Update(Vertices.data(), (uint32_t)(sizeof(MeshVertex) * 3 * Vertices.size()));

		uint32_t* flat_index_array = static_cast<uint32_t*>(glm::value_ptr(Indices.front()));
		vertexArray->GetIndexBuffer()->Update(flat_index_array, (uint32_t)(3 * Indices.size()));
	}
public:
	int entityID = -3; // -3 when component was not given an entityID
	std::vector<MeshVertex> Vertices = { 
		{{ 0.0f, 0.0f, 0.0f }, entityID},
		{{ 1.0f, 0.0f, 0.0f }, entityID},
		{{ 0.0f, 1.0f, 0.0f }, entityID}
	};
	std::vector<glm::uvec3> Indices = { {0, 1, 2} };
	std::shared_ptr<VertexArray> vertexArray = nullptr;
};

struct LineRendererComponent : public Component {
	static const inline char* GetName() { return "LineRendererComponent"; }

	glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
	bool IsLooped = false;

	LineRendererComponent() = default;
	LineRendererComponent(const LineRendererComponent&) = default;
	LineRendererComponent(const glm::vec4& color) :
		Color(color) {}
};

struct MeshRendererComponent : public Component {
	static const inline char* GetName() { return "MeshRendererComponent"; }

	glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
	bool IsTransparent = false;

	MeshRendererComponent() = default;
	MeshRendererComponent(const MeshRendererComponent&) = default;
	MeshRendererComponent(const glm::vec4& color, const bool& isTransparent) :
		Color(color), IsTransparent(isTransparent) {}
};

struct LineGeneratorComponent : public Component {
	static const inline char* GetName() { return "LineGeneratorComponent"; }

	enum class Type { Rectangle = 0, Ellipse, Ngon, Connector };
	struct Rectangle {
		float width = 2.0f;
		float height = 1.0f;
	};
	struct Ellipse {
		float r1 = 2.0f;
		float r2 = 1.0f;
		int numSamples = 16;
	};
	struct Ngon {
		int numSides = 6;
		float radius = 1.0f;
	};
	struct Connector {
		glm::vec3 p1 = { -1.5f, -0.5f, 0.0f };
		glm::vec3 p2 = {  1.5f,  0.5f, 0.0f };
		float steepness = 1.0f;
		int numSamples = 32;
	};

	Type type = Type::Rectangle;
	Rectangle rectangle;
	Ellipse ellipse;
	Ngon ngon;
	Connector connector;

	LineGeneratorComponent() {
		CalculateVertices();
	}

	std::shared_ptr<VertexArray>& GetVertexArray() { return vertexArray; }

	void CalculateVertices() {
		Vertices.clear();
		switch (type) {
		case Type::Rectangle: {
			Vertices.push_back({ -rectangle.width * 0.5f, -rectangle.height * 0.5f, 0.0f });
			Vertices.push_back({ rectangle.width * 0.5f, -rectangle.height * 0.5f, 0.0f });
			Vertices.push_back({ rectangle.width * 0.5f,  rectangle.height * 0.5f, 0.0f });
			Vertices.push_back({ -rectangle.width * 0.5f,  rectangle.height * 0.5f, 0.0f });
		}
		break;
		case Type::Ellipse: {
			for (int i = 0; i < ellipse.numSamples; i++) {
				float t = i * 2.0f * (float)M_PI / ellipse.numSamples;
				Vertices.push_back({ ellipse.r1 * cos(t), ellipse.r2 * sin(t), 0.0f });
			}
		}
		break;
		case Type::Ngon: {
			float angle = 2.0f * (float)M_PI / ngon.numSides;
			for (int i = 0; i < ngon.numSides; i++) {
				Vertices.push_back({ cos(angle * i) * ngon.radius, sin(angle * i) * ngon.radius, 0.0f });
			}
		}
		break;
		case Type::Connector: {
			float diffX = (connector.p2.x - connector.p1.x);
			float diffY = (connector.p2.y - connector.p1.y);
			for (int i = 0; i < connector.numSamples; i++) {
				float u = (float)i / connector.numSamples;
				float w = (u - 0.5f) * connector.steepness;
				float v = tanh(w) * 0.5f + 0.5f;

				float x = connector.p1.x + u * diffX;
				float y = connector.p1.y + v * diffY;
				glm::vec3 p{ x, y, 0.0f };
				Vertices.push_back(p);
			}
		}
		break;
		}
		auto lc = LineComponent(Vertices);
		lc.ComputeVertexArray();
		vertexArray = lc.GetVertexArray();
	}
private:
	std::vector<glm::vec3> Vertices;
	std::shared_ptr<VertexArray> vertexArray = nullptr;
};


