#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

SceneCamera::SceneCamera() {
	RecalculateProjection();
}

void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
{
	orthographicSize = size;
	orthographicNear = nearClip;
	orthographicFar = farClip;
	RecalculateProjection();
}

void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
{
	aspectRatio = (float)width / (float)height;
	RecalculateProjection();
}

void SceneCamera::RecalculateProjection() {
	float orthoLeft = -orthographicSize * aspectRatio * 0.5f;
	float orthoRight = orthographicSize * aspectRatio * 0.5f;
	float orthoBottom = -orthographicSize * 0.5f;
	float orthoTop = orthographicSize * 0.5f;

	Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop,
		orthographicNear, orthographicFar);
}
