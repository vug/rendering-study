#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "SceneCamera.h"

struct TagComponent {
	std::string Tag;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag) :
		Tag(tag) {}

};

struct TransformComponent {
	glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::vec3& translation) :
		Translation(translation) {}
	
	glm::mat4 GetTransform() const {
		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
		return glm::translate(glm::mat4(1.0f), Translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), Scale);
	}
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
