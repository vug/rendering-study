#include "Scene.h"

#include <glm/glm.hpp>

#include "Components.h"



Scene::Scene() {

}

Scene::~Scene() {

}

entt::entity Scene::CreateEntity()
{
	return Registry.create();
}

void Scene::OnUpdate(Timestep ts) {
	
}