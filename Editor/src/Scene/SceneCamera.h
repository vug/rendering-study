#pragma once

#include "../Renderer/Camera.h"

class SceneCamera : public Camera {
public:
	enum class ProjectionType { Perspective = 0, Orthographic = 1 };
public:
	SceneCamera();
	virtual ~SceneCamera() = default;

	void SetOrthographic(float size, float nearClip, float farClip);
	void SetViewportSize(uint32_t width, uint32_t height);
	float GetOrthographicSize() const { return orthographicSize; }
	void SetOrthographicSize(float size) { orthographicSize = size; RecalculateProjection(); }

	float GetOrthographicNearClip() const { return orthographicNear; }
	void SetOrthographicNearClip(float nearClip) { orthographicNear = nearClip; RecalculateProjection(); }

	float GetOrthographicFarClip() const { return orthographicFar; }
	void SetOrthographicFarClip(float farClip) { orthographicFar = farClip; RecalculateProjection(); }

	ProjectionType GetProjectionType() const { return projectionType; }
	void SetProjectionType(ProjectionType type) { projectionType = type; }
private:
	void RecalculateProjection();
private:
	ProjectionType projectionType = ProjectionType::Orthographic;
	float orthographicSize = 10.f;
	float orthographicNear = -1.0f, orthographicFar = 1.0f;

	float aspectRatio = 0.0f;
};
