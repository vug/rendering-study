#pragma once

#include <utility>

#include <glm/glm.hpp>

#include "Camera.h"
#include "../Listener.h"
#include "../Timestep.h"

// Taken from https://github.com/TheCherno/Hazel/commit/96c57dadf5c3616ba3613034dfb63d3918590359 and modified

class EditorCamera : public Camera, public ScrollListener
{
public:
	EditorCamera() = default;
	EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

	void OnUpdate(Timestep ts);

	inline float GetDistance() const { return m_Distance; }
	inline void SetDistance(float distance) { m_Distance = distance; }

	inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

	const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	glm::mat4 GetViewProjection() const { return Projection * m_ViewMatrix; }

	glm::vec3 GetUpDirection() const;
	glm::vec3 GetRightDirection() const;
	glm::vec3 GetForwardDirection() const;
	const glm::vec3& GetPosition() const { return m_Position; }
	glm::quat GetOrientation() const;
	glm::vec3 GetFocalPoint() const { return m_FocalPoint; }

	float GetPitch() const { return m_Pitch; }
	float GetYaw() const { return m_Yaw; }
private:
	void UpdateProjection();
	void UpdateView();

	virtual void OnMouseScroll(float xOffset, float yOffset) override;

	void MousePan(const glm::vec2& delta);
	void MouseRotate(const glm::vec2& delta);
	void MouseZoom(float delta);

	glm::vec3 CalculatePosition() const;

	std::pair<float, float> PanSpeed() const;
	float RotationSpeed() const;
	float ZoomSpeed() const;
private:
	float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

	glm::mat4 m_ViewMatrix;
	glm::vec3 m_Position = { 0.0f, 0.0f, 5.0f };
	glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

	glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

	float m_Distance = 5.0f;
	float m_Pitch = 0.0f, m_Yaw = 0.0f;

	float m_ViewportWidth = 1280, m_ViewportHeight = 720;
};