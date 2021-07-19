#pragma once

#include <filesystem>

#include <yaml-cpp/yaml.h>

#include "Scene.h"

class SceneSerializer {
public:
	SceneSerializer(const std::shared_ptr<Scene>& scene);

	void Serialize(const std::filesystem::path& filepath);

	bool Deserialize(const std::filesystem::path& filepath);
	entt::entity DeserializeEntity(YAML::Node node);
private:
	std::shared_ptr<Scene> scene;
};
