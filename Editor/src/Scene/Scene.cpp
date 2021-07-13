#include<iostream>

#include "Scene.h"

#include <glm/glm.hpp>

#include "Components.h"
#include "../Renderer/Renderer.h"

Scene::Scene() {

}

Scene::~Scene() {

}

// Creates an Entity and gives a Tag and Transform component
entt::entity Scene::CreateEntity(const std::string& name) {
	entt::entity entity = Registry.create();
	Registry.emplace<TransformComponent>(entity);
	auto& tag = Registry.emplace<TagComponent>(entity, name);
	tag.Tag = name.empty() ? "UnnamedObject" : name;
	return entity;
}

void Scene::OnUpdate(Timestep ts) {
	Camera* mainCamera = nullptr;
	glm::mat4* cameraTransform = nullptr;

	{
		auto view = Registry.view<TransformComponent, CameraComponent>();
		for (auto [entity, transform, camera] : view.each()) {
			if (camera.Primary) {
				mainCamera = &camera.Camera;
				cameraTransform = &transform.Transform;
				break;
			}
		}
	}
	if (mainCamera) {
		Renderer::BeginScene(mainCamera->GetProjection(), *cameraTransform);

		auto view = Registry.view<TransformComponent, QuadRendererComponent>();
		for (auto [entity, transform, quad] : view.each()) {
			Renderer::DrawFlatQuad(transform, quad.Color);
		}

		Renderer::EndScene();
	}
}