#include "SceneHierarchyPanel.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

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

	// Right-click on a blank space
	if (ImGui::BeginPopupContextWindow("testing", ImGuiMouseButton_Right, false)) {
		if (ImGui::MenuItem("Create Empty Entity")) {
			context->CreateEntity("Unnamed Entity");
		}
		ImGui::EndPopup();
	}
	ImGui::End();

	ImGui::Begin("Properties");
	if (selectionContext != entt::null) {
		DrawComponents(selectionContext);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");
		
		if (ImGui::BeginPopup("AddComponent")) {
			if (ImGui::MenuItem("Camera")) {
				context->Reg().emplace<CameraComponent>(selectionContext);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Quad Renderer")) {
				context->Reg().emplace<QuadRendererComponent>(selectionContext);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
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
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
		selectionContext = entity;
	}

	bool isEntityDeleted = false;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Delete Entity")) {
			isEntityDeleted = true;
			if (selectionContext == entity) {
				selectionContext = entt::null;
			}
		}
		ImGui::EndPopup();
	}

	if (opened) {
		ImGui::TreePop();
	}

	if (isEntityDeleted) {
		context->DestroyEntity(entity);
	}
}

static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
	bool fancy = true;
	if (!fancy) {
		ImGui::PushID(label.c_str());
		ImGui::Text(label.c_str()); ImGui::SameLine();
		float& x = values[0];
		ImGui::DragFloat("X", &(values.x), 0.1f, 0.0f, 0.0f, "%.3f"); ImGui::SameLine();
		float& y = values[1];
		ImGui::DragFloat("Y", &y); ImGui::SameLine();
		ImGui::DragFloat("Z", &values.z);
		ImGui::PopID();
		return;
	
	}

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	// https://coolors.co/ff595e-ffca3a-8ac926-1982c4-6a4c93
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 0.34901961f, 0.36862745f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 1.000000f, 0.521569f, 0.537255f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 1.000000f, 0.200000f, 0.227451f, 1.0f });

	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.541176f, 0.788235f, 0.149020f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.631373f, 0.858824f, 0.262745f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.462745f, 0.670588f, 0.129412f, 1.0f });
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.098039f, 0.509804f, 0.768627f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.149020f, 0.607843f, 0.890196f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.082353f, 0.423529f, 0.635294f, 1.0f });
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

bool SceneHierarchyPanel::AddComponentSettingsButton() {
	ImGui::SameLine(ImGui::GetWindowWidth() - 35.0f);
	if (ImGui::Button("...", ImVec2{ 30, 20 }))
	{
		ImGui::OpenPopup("ComponentSettings");
	}
	//ImGui::PopStyleVar();

	bool removeComponent = false;
	if (ImGui::BeginPopup("ComponentSettings"))
	{
		if (ImGui::MenuItem("Remove component"))
			removeComponent = true;

		ImGui::EndPopup();
	}
	return removeComponent;
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

	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

	if (handle.all_of<TransformComponent>()) {
		if (ImGui::TreeNodeEx("Transform", treeNodeFlags)) {
			auto& transform = handle.get<TransformComponent>();
			DrawVec3Control("Translation", transform.Translation);
			glm::vec3 rotation = glm::degrees(transform.Rotation);
			DrawVec3Control("Rotation", rotation);
			transform.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", transform.Scale, 1.0f);
			ImGui::TreePop();
		}
	}

	if (handle.all_of<CameraComponent>()) {
		bool isOpen = ImGui::TreeNodeEx("Camera", treeNodeFlags);
		bool shouldRemove = AddComponentSettingsButton();

		if (isOpen) {
			auto& cameraComponent = handle.get<CameraComponent>();
			auto& camera = handle.get<CameraComponent>().Camera;

			ImGui::Checkbox("Primary", &cameraComponent.Primary);
			
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
				float verticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if (ImGui::DragFloat("Vertical FOV", &verticalFOV)) {
					camera.SetPerspectiveVerticalFOV(glm::radians(verticalFOV));
				}

				float near = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near", &near)) {
					camera.SetPerspectiveNearClip(near);
				}

				float far = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far", &far)) {
					camera.SetPerspectiveFarClip(far);
				}
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

				ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
			}

			ImGui::TreePop();
		}

		if (shouldRemove)
			context->Reg().remove<CameraComponent>(entity);
	}

	if (handle.all_of<QuadRendererComponent>()) {
		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		bool isOpen = ImGui::TreeNodeEx("Quad Renderer", treeNodeFlags);
		bool shouldRemove = AddComponentSettingsButton();

		if (isOpen) {
			glm::vec4& color = handle.get<QuadRendererComponent>().Color;
			ImGui::ColorEdit4("Color", glm::value_ptr(color));
			ImGui::TreePop();
		}
		if (shouldRemove)
			context->Reg().remove<QuadRendererComponent>(entity);
	}
}