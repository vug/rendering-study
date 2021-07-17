#include "SceneSerializer.h"

#include <iostream>
#include <fstream>

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
		out << Flow;
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

static void SerializeEntity(YAML::Emitter& out, entt::basic_handle<entt::entity> handle) {
	out << YAML::BeginMap; // Entity
	out << YAML::Key << "Entity" << YAML::Value << "12837192831273";
	
	if (handle.all_of<TagComponent>()) {
		out << YAML::Key << "TagComponent";
		out << YAML::BeginMap; // Component

		TagComponent& tc = handle.get<TagComponent>();
		out << YAML::Key << "Tag" << YAML::Value << tc.Tag;

		out << YAML::EndMap; // Component
	}

	if (handle.all_of<TransformComponent>()) {
		out << YAML::Key << "TransformComponent";
		out << YAML::BeginMap; // Component

		TransformComponent& tc = handle.get<TransformComponent>();
		out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
		out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
		out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

		out << YAML::EndMap; // Component
	}

	if (handle.all_of<CameraComponent>()) {
		out << YAML::Key << "CameraComponent";
		out << YAML::BeginMap; // Component

		CameraComponent& cc = handle.get<CameraComponent>();
		auto& camera = cc.Camera;
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

		out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
		out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;

		out << YAML::EndMap; // Component
	}

	if (handle.all_of<QuadRendererComponent>()) {
		out << YAML::Key << "QuadRendererComponent";
		out << YAML::BeginMap; // Component

		QuadRendererComponent& qrc = handle.get<QuadRendererComponent>();
		out << YAML::Key << "Color" << YAML::Value << qrc.Color;

		out << YAML::EndMap; // Component
	}

	if (handle.all_of<LineComponent>()) {
		out << YAML::Key << "LineComponent";
		out << YAML::BeginMap; // Component

		LineComponent& lc = handle.get<LineComponent>();
		out << YAML::Key << "Vertices" << YAML::Value;
		out << YAML::BeginSeq; // Vertices
		for (auto& v : lc.Vertices) {
			out << v;
		}
		out << YAML::EndSeq;

		out << YAML::EndMap; // Component
	}

	if (handle.all_of<LineRendererComponent>()) {
		out << YAML::Key << "LineRendererComponent";
		out << YAML::BeginMap; // Component

		LineRendererComponent& lrc = handle.get<LineRendererComponent>();
		out << YAML::Key << "Color" << YAML::Value << lrc.Color;
		out << YAML::Key << "IsLooped" << YAML::Value << lrc.IsLooped;

		out << YAML::EndMap; // Component
	}

	if (handle.all_of<LineGeneratorComponent>()) {
		out << YAML::Key << "LineGeneratorComponent";
		out << YAML::BeginMap; // Component

		LineGeneratorComponent& lgc = handle.get<LineGeneratorComponent>();
		out << YAML::Key << "Type" << YAML::Value << (int)lgc.type;

		out << YAML::Key << "Rectangle" << YAML::Value;
		out << YAML::BeginMap; // Rectangle
		out << YAML::Key << "width" << YAML::Value << lgc.rectangle.width;
		out << YAML::Key << "height" << YAML::Value << lgc.rectangle.height;
		out << YAML::EndMap; // Rectangle

		out << YAML::Key << "Ellipse" << YAML::Value;
		out << YAML::BeginMap; // Ellipse
		out << YAML::Key << "r1" << YAML::Value << lgc.ellipse.r1;
		out << YAML::Key << "r2" << YAML::Value << lgc.ellipse.r2;
		out << YAML::Key << "numSamples" << YAML::Value << lgc.ellipse.numSamples;
		out << YAML::EndMap; // Ellipse

		out << YAML::Key << "Ngon" << YAML::Value;
		out << YAML::BeginMap; // Ngon
		out << YAML::Key << "numSides" << YAML::Value << lgc.ngon.numSides;
		out << YAML::Key << "radius" << YAML::Value << lgc.ngon.radius;
		out << YAML::EndMap; // Ngon

		out << YAML::Key << "Connector" << YAML::Value;
		out << YAML::BeginMap; // Connector
		out << YAML::Key << "p1" << YAML::Value << lgc.connector.p1;
		out << YAML::Key << "p2" << YAML::Value << lgc.connector.p2;
		out << YAML::Key << "steepness" << YAML::Value << lgc.connector.steepness;
		out << YAML::Key << "numSamples" << YAML::Value << lgc.connector.numSamples;
		out << YAML::EndMap; // Connector

		out << YAML::EndMap; // Component
	}

	out << YAML::EndMap; // Entity
}

void SceneSerializer::Serialize(const std::filesystem::path& filepath) {
	YAML::Emitter out;
	out << YAML::BeginMap; // Scene
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
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
	return false;
}
