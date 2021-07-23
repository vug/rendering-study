#pragma once

#include <memory>

#include "entt/entt.hpp"

#include "Components.h"
#include "../Timestep.h"

class Scene {
public:
	Scene();
	~Scene();

	entt::entity CreateEntity(const std::string& name = std::string());
	void DestroyEntity(entt::entity entity);

	entt::registry& Reg() { return Registry; }

	void OnUpdate(Timestep ts);
	void OnViewportResize(uint32_t width, uint32_t height);

	entt::entity GetPrimaryCameraEntity();
	bool renderWireframe = false;
	bool renderOnlyFront = false;
	bool renderFlatShading = true;
private:
	void OnCameraCreated(entt::registry& registry, entt::entity entity);
private:
	entt::registry Registry;
	// hack to prevent division by zero before first computation
	uint32_t viewportWidth = 1, viewportHeight = 1;
	
	friend class SceneHierarchyPanel;
	friend class SceneSerializer;
};
