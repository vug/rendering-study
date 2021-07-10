#pragma once

#include "Renderer/OrthographicCamera.h"

#include "Application.h"
#include "Input.h"

class OrthographicCameraController : public ScrollListener {
public:
	OrthographicCameraController(Application& app, float aspectRatio); // AspectRatio * 2 units

	void OnUpdate(Timestep ts);
	void OnScrollUpdate(float xOffset, float yOffset) override;

	OrthographicCamera& GetCamera() { return camera; }
	const OrthographicCamera& GetCamera() const { return camera; }

	const float GetZoomLevel() const { return zoomLevel; }
private:
	bool OnWindowResized();
private:
	Application& app;
	float aspectRatio;
	float zoomLevel = 1.0f;
	OrthographicCamera camera;
	//OrthographicCamera camera = OrthographicCamera(-1.6f, 1.6f, -0.9f, 0.9f);
	glm::vec3 cameraPosition = { 0.0f, 0.0f, 0.0f };
	float cameraTranslationSpeed = 5.0f;
	float cameraRotation = 0.0f;
	float cameraRotationSpeed = 180.0f;
};
