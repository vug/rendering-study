#pragma once

#include <entt/entt.hpp>

#include "Scene.h"

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel() = default;
	SceneHierarchyPanel(const std::shared_ptr<Scene> scene);

	void SetContext(const std::shared_ptr<Scene> scene);
	entt::entity GetSelectedEntity() { return selectionContext; }

	void OnImguiRender();
private:
	void DrawEntityNode(entt::entity entity);
	void DrawComponents(entt::entity entity);
	static bool AddComponentSettingsButton();
private:
	std::shared_ptr<Scene> context;
	entt::entity selectionContext = entt::null;
};
