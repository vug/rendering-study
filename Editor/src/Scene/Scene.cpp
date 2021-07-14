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
	glm::mat4 cameraTransform;

	{
		auto view = Registry.view<TransformComponent, CameraComponent>();
		for (auto [entity, transform, camera] : view.each()) {
			if (camera.Primary) {
				mainCamera = &camera.Camera;
				cameraTransform = transform.GetTransform();
				break;
			}
		}
	}
	if (mainCamera) {
		Renderer::BeginScene(mainCamera->GetProjection(), cameraTransform);

		auto view = Registry.view<TransformComponent, QuadRendererComponent>();
		for (auto [entity, transform, quad] : view.each()) {
			Renderer::DrawFlatQuad(transform.GetTransform(), quad.Color);
		}

		Renderer::EndScene();
	}
}

void Scene::OnViewportResize(uint32_t width, uint32_t height) {
	auto view = Registry.view<CameraComponent>();
	for (auto [entity, camera] : view.each()) {
		if (!camera.FixedAspectRatio) {
			camera.Camera.SetViewportSize(width, height);
		}
	}
}