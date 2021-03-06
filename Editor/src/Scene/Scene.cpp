#include <algorithm>
#include <iostream>
#include <map>

#include "Scene.h"

#include <glm/glm.hpp>

#include "Components.h"
#include "../Renderer/Renderer.h"

void Scene::OnCameraCreated(entt::registry& registry, entt::entity entity) {
	CameraComponent& cc = registry.get<CameraComponent>(entity);
	cc.Camera.SetViewportSize(viewportWidth, viewportHeight);
}

void Scene::OnMeshCreated(entt::registry& registry, entt::entity entity) {
	MeshComponent& comp = registry.get<MeshComponent>(entity);
	comp.entityID = (int)entity;
	comp.ComputeVertexArray();
}

void Scene::OnMeshObjLoaderCreated(entt::registry& registry, entt::entity entity) {
	MeshObjLoaderComponent& comp = registry.get<MeshObjLoaderComponent>(entity);
	comp.meshComponent.entityID = (int)entity;
}

Scene::Scene() {
	Registry.on_construct<CameraComponent>().connect<&Scene::OnCameraCreated>(this);
	Registry.on_construct<MeshComponent>().connect<&Scene::OnMeshCreated>(this);
	Registry.on_construct<MeshObjLoaderComponent>().connect<&Scene::OnMeshObjLoaderCreated>(this);
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

void Scene::OnUpdate(Timestep ts, EditorCamera& editorCamera) {
	RenderCommand::Init(renderWireframe, renderOnlyFront);

	std::vector<Renderer::LightInfo> lightInfos;
	auto viewLights = Registry.view<TransformComponent, LightComponent>();
	for (auto [entity, transform, light] : viewLights.each()) {
		lightInfos.push_back({transform.Translation, light.intensity});
	}
 
	Camera* sceneCamera = nullptr;
	glm::mat4 cameraTransform;
	glm::vec3 cameraTranslation;
	{
		auto view = Registry.view<TransformComponent, CameraComponent>();
		for (auto [entity, transform, camera] : view.each()) {
			if (camera.Primary) {
				sceneCamera = &camera.Camera;
				cameraTransform = transform.GetTransform();
				cameraTranslation = transform.Translation;
				break;
			}
		}
	}
	if (sceneCamera) {
		Renderer::BeginScene(sceneCamera->GetProjection(), cameraTransform, lightInfos);
	}
	else {
		Renderer::BeginScene(editorCamera.GetProjection(), editorCamera.GetViewMatrix(), lightInfos);
		cameraTranslation = editorCamera.GetPosition();
	}
	
	auto view2 = Registry.view<TransformComponent, LineComponent, LineRendererComponent>();
	for (auto [entity, transform, line, lineRenderer] : view2.each()) {
		Renderer::DrawLines(line.GetVertexArray(), transform.GetTransform(), lineRenderer.Color, lineRenderer.IsLooped);
	}

	auto view3 = Registry.view<TransformComponent, LineGeneratorComponent, LineRendererComponent>();
	for (auto [entity, transform, line, lineRenderer] : view3.each()) {
		Renderer::DrawLines(line.GetVertexArray(), transform.GetTransform(), lineRenderer.Color, lineRenderer.IsLooped);
	}

	auto view4 = Registry.view<TransformComponent, MeshRendererComponent>();
	class TriangleParams {
	public:
		MeshComponent* mesh;
		MeshRendererComponent* meshRenderer;
		std::shared_ptr<Shader> shader;
		TransformComponent* transform;
		uint32_t triangleNo;
		float dist = 100000.0f;

		TriangleParams(const TriangleParams&) = default;
	};
	std::vector<TriangleParams> triangles;
	glDepthMask(GL_TRUE);
	for (auto [entity, transform, meshRenderer] : view4.each()) {
		std::shared_ptr<Shader> shader = renderFlatShading ?
			ShaderLibrary::Instance().Get("FlatShader") :
			ShaderLibrary::Instance().Get("SolidColor");

		entt::basic_handle handle = { Registry, entity };
		MeshComponent* mesh = nullptr;
		if (handle.all_of<MeshComponent>()) {
			mesh = &handle.get<MeshComponent>();
		}
		else if (handle.all_of<MeshObjLoaderComponent>()) {
			mesh = &handle.get<MeshObjLoaderComponent>().meshComponent;
		}
		else {
			assert(false); // An entity with MeshRendererComponent should either have MeshComponent or a MeshObjLoaderComponent
		}

		// Render opaque objects one draw call per mesh using regular depth buffer
		if (!meshRenderer.IsTransparent) {
			Renderer::DrawMesh(*mesh, meshRenderer, shader, transform);
		}

		// store transparent object triangles in a vector
		else {
			int indexCount = mesh->vertexArray->GetIndexBuffer()->GetCount();
			int numTriangles = indexCount / 3;
			for (int i = 0; i < numTriangles; i++) {
				float minDistOfTriangle = 1000000.0f;
				glm::uvec3 triangleVertexIndices = mesh->Indices[i];
				for (int j = 0; j < 3; j++) {
					auto ix = triangleVertexIndices[j];
					glm::vec3& v = mesh->Vertices[ix].Position;
					glm::vec3 worldVertexPos = glm::vec3(transform.GetTransform() * glm::vec4{ v.x, v.y, v.z, 1.0 });
					minDistOfTriangle = std::min(minDistOfTriangle, glm::length(worldVertexPos - cameraTranslation));
				}
				TriangleParams tp = TriangleParams{ mesh, &meshRenderer, shader, &transform, (uint32_t)i, minDistOfTriangle };
				triangles.push_back(tp);
			}
		}
	}

	// sort transparent triangles by distance to camera
	// draw them without writing to depth buffer
	glDepthMask(GL_FALSE);
	std::sort(triangles.rbegin(), triangles.rend(), 
		[](const TriangleParams& tp1, const TriangleParams& tp2) { return tp1.dist < tp2.dist; });
	for (TriangleParams& tp : triangles) {
		Renderer::DrawMeshTriangle(*tp.mesh, *tp.meshRenderer, tp.shader, *tp.transform, tp.triangleNo);
	}

	Renderer::EndScene();
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
