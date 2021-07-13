#pragma once

#include <glm/glm.hpp>

#include "SceneCamera.h"

struct TagComponent {
	std::string Tag;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag) :
		Tag(tag) {}

};

struct TransformComponent {
	glm::mat4 Transform{ 1.0f };

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::mat4& transform) :
		Transform(transform) {}
	// allows to give a TransformComponent object to a function that expects a glm::mat4 :-O
	operator glm::mat4& () { return Transform; }
	operator const glm::mat4& () const { return Transform; }
};

struct QuadRendererComponent {
	glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

	QuadRendererComponent() = default;
	QuadRendererComponent(const QuadRendererComponent&) = default;
	QuadRendererComponent(const glm::vec4& color) :
		Color(color) {}
};

struct CameraComponent {
	SceneCamera Camera;
	bool Primary = true; // TODO: think about moving to Scene
	bool FixedAspectRatio = false;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
};
