#pragma once

#include "entt/entt.hpp"

#include "../Timestep.h"

class Scene {
public:
	Scene();
	~Scene();

	entt::entity CreateEntity();

	entt::registry& Reg() { return Registry; }

	void OnUpdate(Timestep ts);
private:
	entt::registry Registry;
};
