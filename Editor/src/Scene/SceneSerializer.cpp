#include "SceneSerializer.h"

#include <iostream>
#include <fstream>
#include <type_traits>

#include <yaml-cpp/yaml.h>

#include "Components.h"

namespace YAML {
	// Introduce encode/decode functions to YAML::Node class for glm::vec3/4
	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::uvec3> {
		static Node encode(const glm::uvec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::uvec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<uint32_t>();
			rhs.y = node[1].as<uint32_t>();
			rhs.z = node[2].as<uint32_t>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const glm::vec3& v) {
		out << Flow; // keeps items in a single line
		out << BeginSeq << v.x << v.y << v.z << EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::vec4& v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << v.z << v.w << EndSeq;
		return out;
	}
}

SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene)
	: scene(scene) {
}

namespace ComponentSerializer {

	static void serialize(YAML::Emitter& out, TagComponent& comp) {
		out << YAML::Key << "Tag" << YAML::Value << comp.Tag;
	}

	static void serialize(YAML::Emitter& out, TransformComponent& comp) {
		out << YAML::Key << "Translation" << YAML::Value << comp.Translation;
		out << YAML::Key << "Rotation" << YAML::Value << comp.Rotation;
		out << YAML::Key << "Scale" << YAML::Value << comp.Scale;
	}

	static void serialize(YAML::Emitter& out, CameraComponent& comp) {
		auto& camera = comp.Camera;
		out << YAML::Key << "Camera" << YAML::Value;
		out << YAML::BeginMap; // Camera
		out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
		out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
		out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
		out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
		out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
		out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
		out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
		out << YAML::EndMap; // Camera

		out << YAML::Key << "Primary" << YAML::Value << comp.Primary;
		out << YAML::Key << "FixedAspectRatio" << YAML::Value << comp.FixedAspectRatio;
	}

	static void serialize(YAML::Emitter& out, QuadRendererComponent& comp) {
		out << YAML::Key << "Color" << YAML::Value << comp.Color;
	}

	static void serialize(YAML::Emitter& out, LineComponent& comp) {
		out << YAML::Key << "Vertices" << YAML::Value;
		out << YAML::BeginSeq; // Vertices
		for (auto& v : comp.Vertices) {
			out << v;
		}
		out << YAML::EndSeq;
	}

	static void serialize(YAML::Emitter& out, LineRendererComponent& comp) {
		out << YAML::Key << "Color" << YAML::Value << comp.Color;
		out << YAML::Key << "IsLooped" << YAML::Value << comp.IsLooped;
	}

	static void serialize(YAML::Emitter& out, LineGeneratorComponent& comp) {
		out << YAML::Key << "Type" << YAML::Value << (int)comp.type;

		out << YAML::Key << "Rectangle" << YAML::Value;
		out << YAML::BeginMap; // Rectangle
		out << YAML::Key << "width" << YAML::Value << comp.rectangle.width;
		out << YAML::Key << "height" << YAML::Value << comp.rectangle.height;
		out << YAML::EndMap; // Rectangle

		out << YAML::Key << "Ellipse" << YAML::Value;
		out << YAML::BeginMap; // Ellipse
		out << YAML::Key << "r1" << YAML::Value << comp.ellipse.r1;
		out << YAML::Key << "r2" << YAML::Value << comp.ellipse.r2;
		out << YAML::Key << "numSamples" << YAML::Value << comp.ellipse.numSamples;
		out << YAML::EndMap; // Ellipse

		out << YAML::Key << "Ngon" << YAML::Value;
		out << YAML::BeginMap; // Ngon
		out << YAML::Key << "numSides" << YAML::Value << comp.ngon.numSides;
		out << YAML::Key << "radius" << YAML::Value << comp.ngon.radius;
		out << YAML::EndMap; // Ngon

		out << YAML::Key << "Connector" << YAML::Value;
		out << YAML::BeginMap; // Connector
		out << YAML::Key << "p1" << YAML::Value << comp.connector.p1;
		out << YAML::Key << "p2" << YAML::Value << comp.connector.p2;
		out << YAML::Key << "steepness" << YAML::Value << comp.connector.steepness;
		out << YAML::Key << "numSamples" << YAML::Value << comp.connector.numSamples;
		out << YAML::EndMap; // Connector
	}

	static void serialize(YAML::Emitter& out, MeshComponent& comp) {
		out << YAML::Key << "Vertices" << YAML::Value;
		out << YAML::BeginSeq; // Vertices
		for (auto& v : comp.Vertices) {
			out << v;
		}
		out << YAML::EndSeq; // Vertices

		out << YAML::Key << "Indices" << YAML::Value;
		out << YAML::BeginSeq; // Indices
		for (auto& triplet : comp.Indices) {
			out << triplet;
		}
		out << YAML::EndSeq; // Indices
	}

	static void serialize(YAML::Emitter& out, MeshRendererComponent& comp) {
		out << YAML::Key << "Color" << YAML::Value << comp.Color;
		out << YAML::Key << "IsTransparent" << YAML::Value << comp.IsTransparent;
	}

	template <typename TComp, typename = std::enable_if_t<std::is_base_of_v<Component, TComp>>>
	static void serializeIfExists(YAML::Emitter& out, entt::basic_handle<entt::entity> handle) {
		if (handle.all_of<TComp>()) {
			auto& comp = handle.get<TComp>();
			out << YAML::Key << TComp::GetName();
			out << YAML::BeginMap; // Component
			serialize(out, comp);
			out << YAML::EndMap; // Component
		}
	}

	static void deserialize(YAML::Node node, TransformComponent& comp) {
		comp.Translation = node["Translation"].as<glm::vec3>();
		comp.Rotation = node["Rotation"].as<glm::vec3>();
		comp.Scale = node["Scale"].as<glm::vec3>();
	}

	static void deserialize(YAML::Node node, CameraComponent& comp) {
		YAML::Node cameraProps = node["Camera"];
		comp.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

		comp.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
		comp.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
		comp.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

		comp.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
		comp.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
		comp.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

		comp.Primary = node["Primary"].as<bool>();
		comp.FixedAspectRatio = node["FixedAspectRatio"].as<bool>();
	}

	static void deserialize(YAML::Node node, QuadRendererComponent& comp) {
		comp.Color = node["Color"].as<glm::vec4>();
	}

	static void deserialize(YAML::Node node, LineComponent& comp) {
		std::vector<glm::vec3> vertices;
		for (auto vertex : node["Vertices"]) {
			vertices.push_back(vertex.as<glm::vec3>());
		}
		comp.Vertices = vertices;
		comp.ComputeVertexArray();
	}

	static void deserialize(YAML::Node node, LineGeneratorComponent& comp) {
		comp.type = (LineGeneratorComponent::Type)node["Type"].as<int>();

		auto rectangle = node["Rectangle"];
		comp.rectangle.width = rectangle["width"].as<float>();
		comp.rectangle.height = rectangle["height"].as<float>();

		auto ellipse = node["Ellipse"];
		comp.ellipse.r1 = ellipse["r1"].as<float>();
		comp.ellipse.r2 = ellipse["r2"].as<float>();
		comp.ellipse.numSamples = ellipse["numSamples"].as<int>();

		auto ngon = node["Ngon"];
		comp.ngon.numSides = ngon["numSides"].as<int>();
		comp.ngon.radius = ngon["radius"].as<float>();

		auto connector = node["Connector"];
		comp.connector.p1 = connector["p1"].as<glm::vec3>();
		comp.connector.p2 = connector["p2"].as<glm::vec3>();
		comp.connector.steepness = connector["steepness"].as<float>();
		comp.connector.numSamples = connector["numSamples"].as<int>();

		comp.CalculateVertices();
	}

	static void deserialize(YAML::Node node, LineRendererComponent& comp) {
		comp.Color = node["Color"].as<glm::vec4>();
		comp.IsLooped = node["IsLooped"].as<bool>();
	}

	static void deserialize(YAML::Node node, MeshComponent& comp) {
		std::vector<glm::vec3> vertices = {};
		std::vector<glm::uvec3> indices = {};
		for (auto vertex : node["Vertices"]) {
			vertices.push_back(vertex.as<glm::vec3>());
		}
		for (auto index : node["Indices"]) {
			indices.push_back(index.as<glm::uvec3>());
		}
		comp.Vertices = vertices;
		comp.Indices = indices;
		comp.ComputeVertexArray();
	}

	static void deserialize(YAML::Node node, MeshRendererComponent& comp) {
		comp.Color = node["Color"].as<glm::vec4>();
		comp.IsTransparent = node["IsTransparent"].as<bool>();
	}

	template <typename TComp, typename = std::enable_if_t<std::is_base_of_v<Component, TComp>>>
	static void deserializeIfExists(YAML::Node nodeEntity, entt::basic_handle<entt::entity> handle) {
		YAML::Node nodeComp = nodeEntity[TComp::GetName()];
		if (!nodeComp) return;
		TComp& comp = std::is_same_v<TComp, TransformComponent> ? 
			handle.get<TComp>() : // assume TransformComponent exists on the entity
			handle.emplace<TComp>();
		deserialize(nodeComp, comp);
	}
}

static void SerializeEntity(YAML::Emitter& out, entt::basic_handle<entt::entity> handle) {
	out << YAML::BeginMap; // Entity
	out << YAML::Key << "Entity" << YAML::Value << "12837192831273";
	
	ComponentSerializer::serializeIfExists<TagComponent>(out, handle);
	ComponentSerializer::serializeIfExists<TransformComponent>(out, handle);
	ComponentSerializer::serializeIfExists<CameraComponent>(out, handle);
	ComponentSerializer::serializeIfExists<QuadRendererComponent>(out, handle);
	ComponentSerializer::serializeIfExists<LineComponent>(out, handle);
	ComponentSerializer::serializeIfExists<LineRendererComponent>(out, handle);
	ComponentSerializer::serializeIfExists<LineGeneratorComponent>(out, handle);
	ComponentSerializer::serializeIfExists<MeshComponent>(out, handle);
	ComponentSerializer::serializeIfExists<MeshRendererComponent>(out, handle);

	out << YAML::EndMap; // Entity
}

entt::entity SceneSerializer::DeserializeEntity(YAML::Node node) {
	uint64_t uuid = node["Entity"].as<uint64_t>(); // TODO

	std::string tag;
	auto tagComponent = node[TagComponent::GetName()];
	if (tagComponent) {
		tag = tagComponent["Tag"].as<std::string>();
	}

	entt::entity deserializedEntity = scene->CreateEntity(tag);
	entt::basic_handle deserializedHandle = entt::basic_handle{ scene->Registry, deserializedEntity };

	ComponentSerializer::deserializeIfExists<TransformComponent>(node, deserializedHandle);
	ComponentSerializer::deserializeIfExists<QuadRendererComponent>(node, deserializedHandle);
	ComponentSerializer::deserializeIfExists<CameraComponent>(node, deserializedHandle);
	ComponentSerializer::deserializeIfExists<LineComponent>(node, deserializedHandle);
	ComponentSerializer::deserializeIfExists<LineRendererComponent>(node, deserializedHandle);
	ComponentSerializer::deserializeIfExists<LineGeneratorComponent>(node, deserializedHandle);
	ComponentSerializer::deserializeIfExists<MeshComponent>(node, deserializedHandle);
	ComponentSerializer::deserializeIfExists<MeshRendererComponent>(node, deserializedHandle);

	return deserializedEntity;
}

void SceneSerializer::Serialize(const std::filesystem::path& filepath) {
	YAML::Emitter out;
	out << YAML::BeginMap; // Scene
	out << YAML::Key << "Scene" << YAML::Value << "Untitled Scene";
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq; // Entities

	scene->Reg().each([&](entt::entity entity) {
		entt::basic_handle handle{ scene->Reg(), entity };
		if (!handle.valid()) {
			assert(false); // Invalid entity. Can't serialize the scene.
		}
		SerializeEntity(out, handle);
	});

	out << YAML::EndSeq; // Entities
	out << YAML::EndMap; // Scene


	std::ofstream fout(filepath);
	fout << out.c_str();
}

bool SceneSerializer::Deserialize(const std::filesystem::path& filepath) {
	YAML::Node data = YAML::LoadFile(filepath.string());
	if (!data["Scene"])
		return false;

	std::string sceneName = data["Scene"].as<std::string>();

	auto entities = data["Entities"];
	if (!entities)
		return false;

	for (auto entity : entities) {
		DeserializeEntity(entity);
	}
	
	return true;
}
