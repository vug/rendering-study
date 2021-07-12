#include "Scene.h"

#include <glm/glm.hpp>

#include "Components.h"
#include "../Renderer/Renderer.h"

Scene::Scene() {

}

Scene::~Scene() {

}

entt::entity Scene::CreateEntity()
{
	return Registry.create();
}

void Scene::OnUpdate(Timestep ts) {
	auto view = Registry.view<TransformComponent, QuadRendererComponent>();
    for (auto [entity, transform, quad] : view.each()) {
		Renderer::DrawFlatQuad(transform, quad.Color);
    }
}