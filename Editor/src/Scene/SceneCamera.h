#pragma once

#include "../Renderer/Camera.h"

class SceneCamera : public Camera {
public:
	enum class ProjectionType { Perspective = 0, Orthographic = 1 };
public:
	SceneCamera();
	virtual ~SceneCamera() = default;

	void SetOrthographic(float size, float nearClip, float farClip);
	void SetPerspective(float verticalFOV, float nearClip, float farClip);
	void SetViewportSize(uint32_t width, uint32_t height);

	float GetPerspectiveVerticalFOV() const { return perspectiveFOV; }
	void SetPerspectiveVerticalFOV(float verticalFov) { perspectiveFOV = verticalFov; RecalculateProjection(); }
	float GetPerspectiveNearClip() const { return perspectiveNear; }
	void SetPerspectiveNearClip(float nearClip) { perspectiveNear = nearClip; RecalculateProjection(); }
	float GetPerspectiveFarClip() const { return perspectiveFar; }
	void SetPerspectiveFarClip(float farClip) { perspectiveFar = farClip; RecalculateProjection(); }


	float GetOrthographicSize() const { return orthographicSize; }
	void SetOrthographicSize(float size) { orthographicSize = size; RecalculateProjection(); }
	float GetOrthographicNearClip() const { return orthographicNear; }
	void SetOrthographicNearClip(float nearClip) { orthographicNear = nearClip; RecalculateProjection(); }
	float GetOrthographicFarClip() const { return orthographicFar; }
	void SetOrthographicFarClip(float farClip) { orthographicFar = farClip; RecalculateProjection(); }

	ProjectionType GetProjectionType() const { return projectionType; }
	void SetProjectionType(ProjectionType type) { projectionType = type; RecalculateProjection(); }
private:
	void RecalculateProjection();
private:
	ProjectionType projectionType = ProjectionType::Orthographic;

	float perspectiveFOV = glm::radians(45.0f);
	float perspectiveNear = 0.01f, perspectiveFar = 1000.0f;

	float orthographicSize = 10.f;
	float orthographicNear = -1.0f, orthographicFar = 1.0f;

	float aspectRatio = 0.0f;
};
