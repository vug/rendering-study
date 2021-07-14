#pragma once

#include <memory>

#include "entt/entt.hpp"

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
private:
	entt::registry Registry;
	
	friend class SceneHierarchyPanel;
};
