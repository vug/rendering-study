#pragma once

#include "Renderer/OrthographicCamera.h"

#include "Listener.h"
#include "Timestep.h"

class OrthographicCameraController : public ScrollListener, public WindowListener {
public:
	OrthographicCameraController(float aspectRatio); // AspectRatio * 2 units

	void OnUpdate(Timestep ts);
	void OnScrollUpdate(float xOffset, float yOffset) override;
	void OnWindowResize(int width, int height) override;
	void OnViewportResized(float width, float height);

	OrthographicCamera& GetCamera() { return camera; }
	const OrthographicCamera& GetCamera() const { return camera; }

	const float GetZoomLevel() const { return zoomLevel; }
private:
	float aspectRatio;
	float zoomLevel = 1.0f;
	OrthographicCamera camera;
	glm::vec3 cameraPosition = { 0.0f, 0.0f, 0.0f };
	float cameraTranslationSpeed = 5.0f;
	float cameraRotation = 0.0f;
	float cameraRotationSpeed = 180.0f;
};

