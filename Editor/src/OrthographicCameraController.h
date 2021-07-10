#pragma once

#include "Renderer/OrthographicCamera.h"
#include "Application.h"

class OrthographicCameraController {
public:
	OrthographicCameraController(Application& app, float aspectRatio); // AspectRatio * 2 units

	void OnUpdate(Timestep ts);

	OrthographicCamera& GetCamera() { return camera; }
	const OrthographicCamera& GetCamera() const { return camera; }
private:
	bool OnMouseScrolled();
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
