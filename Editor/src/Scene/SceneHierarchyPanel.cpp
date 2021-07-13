#include "SceneHierarchyPanel.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include "Components.h"

SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene> scene) {
	SetContext(scene);
}

void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene> scene) {
	context = scene;
}

void SceneHierarchyPanel::OnImguiRender() {
	ImGui::Begin("Scene Hierarchy");
	context->Registry.each([&](entt::entity entity) {
		DrawEntityNode(entity);
	});

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
		selectionContext = entt::null;
	}
	ImGui::End();

	ImGui::Begin("Properties");
	if (selectionContext != entt::null) {
		DrawComponents(selectionContext);
	}
	ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(entt::entity entity) {
	auto& tc = context->Registry.get<TagComponent>(entity);

	// don't open tree node when click on name, but only on arrow only
	ImGuiTreeNodeFlags flags = ((selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0)
		| ImGuiTreeNodeFlags_OpenOnArrow;
	const char* label = tc.Tag.c_str();
	bool opened = ImGui::TreeNodeEx(label, flags);
	if (ImGui::IsItemClicked()) {
		selectionContext = entity;
	}
	if (opened) {
		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::DrawComponents(entt::entity entity) {
	entt::basic_handle handle{ context->Registry, entity };

	if (handle.all_of<TagComponent>()) {
		std::string& tag = handle.get<TagComponent>().Tag;
		
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), tag.c_str());
		if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
			tag = std::string(buffer);
		}
	}

	if (handle.all_of<TransformComponent>()) {
		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
			glm::mat4& transform = handle.get<TransformComponent>().Transform;
			ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.25f);
			ImGui::TreePop();
		}
	}

	if (handle.all_of<CameraComponent>()) {
		if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
			auto& camera = handle.get<CameraComponent>().Camera;
			
			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {

			}
			else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
				float size = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &size)) {
					camera.SetOrthographicSize(size);
				}

				float near = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near", &near)) {
					camera.SetOrthographicNearClip(near);
				}

				float far = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far", &far)) {
					camera.SetOrthographicFarClip(far);
				}
			}

			ImGui::TreePop();
		}
	}
}
