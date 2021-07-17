#pragma once

#include <filesystem>

#include "Scene.h"

class SceneSerializer {
public:
	SceneSerializer(const std::shared_ptr<Scene>& scene);

	void Serialize(const std::filesystem::path& filepath);

	bool Deserialize(const std::filesystem::path& filepath);
private:
	std::shared_ptr<Scene> scene;
};
