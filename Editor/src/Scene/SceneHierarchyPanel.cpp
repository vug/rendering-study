#include "SceneHierarchyPanel.h"

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
