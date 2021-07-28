#pragma once

#include <entt/entt.hpp>

#include "Scene.h"

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel() = default;
	SceneHierarchyPanel(const std::shared_ptr<Scene> scene);

	void SetContext(const std::shared_ptr<Scene> scene);
	entt::entity GetSelectedEntity() { return selectionContext; }
	void SetSelectedEntity(entt::entity entity) { selectionContext = entity; }

	void OnImguiRender();
private:
	void DrawEntityNode(entt::entity entity);
	void DrawComponents(entt::entity entity);
	static bool AddComponentSettingsButton();
	template <typename TComp> // Has to be a Component
	void DrawComponentUITreeNodeIfExists(entt::basic_handle<entt::entity> handle, void DrawCompParams(TComp&));
private:
	std::shared_ptr<Scene> context;
	entt::entity selectionContext = entt::null;
};
