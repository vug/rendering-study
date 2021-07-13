#pragma once

#include <entt/entt.hpp>

#include "Scene.h"

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel() = default;
	SceneHierarchyPanel(const std::shared_ptr<Scene> scene);

	void SetContext(const std::shared_ptr<Scene> scene);

	void OnImguiRender();
private:
	void DrawEntityNode(entt::entity entity);
private:
	std::shared_ptr<Scene> context;
	entt::entity selectionContext = entt::null;
};
