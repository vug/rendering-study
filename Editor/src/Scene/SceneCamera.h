#pragma once

#include "../Renderer/Camera.h"

class SceneCamera : public Camera {
public:
	SceneCamera();
	virtual ~SceneCamera() = default;

	void SetOrthographic(float size, float nearClip, float farClip);
	void SetViewportSize(uint32_t width, uint32_t height);
	float GetOrthographicSize() const { return orthographicSize; }
	void SetOrthographicSize(float size) { orthographicSize = size; RecalculateProjection(); }
private:
	void RecalculateProjection();
private:
	float orthographicSize = 10.f;
	float orthographicNear = -1.0f, orthographicFar = 1.0f;

	float aspectRatio = 0.0f;
};
