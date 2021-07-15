#include<iostream>

#include "Scene.h"

#include <glm/glm.hpp>

#include "Components.h"
#include "../Renderer/Renderer.h"

void Scene::OnCameraCreated(entt::registry& registry, entt::entity entity) {
	CameraComponent& cc = registry.get<CameraComponent>(entity);
	cc.Camera.SetViewportSize(viewportWidth, viewportHeight);
}

Scene::Scene() {
	Registry.on_construct<CameraComponent>().connect<&Scene::OnCameraCreated>(this);
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

void Scene::DestroyEntity(entt::entity entity) {
	Registry.destroy(entity);
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

		{
			auto view = Registry.view<TransformComponent, QuadRendererComponent>();
			for (auto [entity, transform, quad] : view.each()) {
				Renderer::DrawFlatQuad(transform.GetTransform(), quad.Color);
			}

		}

		{
			auto view = Registry.view<TransformComponent, LineComponent, LineRendererComponent>();
			for (auto [entity, transform, line, lineRenderer] : view.each()) {
				Renderer::DrawLines(line.GetVertexArray(), transform.GetTransform(), lineRenderer.Color, lineRenderer.IsLooped);
			}
		}

		auto view3 = Registry.view<TransformComponent, LineGeneratorComponent, LineRendererComponent>();
		for (auto [entity, transform, line, lineRenderer] : view3.each()) {
			Renderer::DrawLines(line.GetVertexArray(), transform.GetTransform(), lineRenderer.Color, lineRenderer.IsLooped);
		}


		Renderer::EndScene();
	}
}

void Scene::OnViewportResize(uint32_t width, uint32_t height) {
	viewportWidth = width;
	viewportHeight = height;

	// Resize Our non-fixed aspect ratio cameras
	auto view = Registry.view<CameraComponent>();
	for (auto [entity, camera] : view.each()) {
		if (!camera.FixedAspectRatio) {
			camera.Camera.SetViewportSize(width, height);
		}
	}
}

entt::entity Scene::GetPrimaryCameraEntity() {
	auto view = Registry.view<CameraComponent>();
	for (auto [entity, camera] : view.each()) {
		if (camera.Primary) {
			return entity;
		}
	}
	return entt::null;
}
