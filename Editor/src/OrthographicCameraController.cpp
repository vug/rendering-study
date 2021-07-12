#include "OrthographicCameraController.h"

#include <iostream>

#include "Input.h"

OrthographicCameraController::OrthographicCameraController(float aspectRatio)
    : aspectRatio(aspectRatio), camera(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel) {
}

void OrthographicCameraController::OnUpdate(Timestep ts) {
    if (Input::IsKeyHeld(GLFW_KEY_A)) 
        cameraPosition.x -= cameraTranslationSpeed * ts;
    else if (Input::IsKeyHeld(GLFW_KEY_D))
        cameraPosition.x += cameraTranslationSpeed * ts;

    if (Input::IsKeyHeld(GLFW_KEY_W))
        cameraPosition.y += cameraTranslationSpeed * ts;
    else if (Input::IsKeyHeld(GLFW_KEY_S))
        cameraPosition.y -= cameraTranslationSpeed * ts;

    if (Input::IsKeyHeld(GLFW_KEY_Q))
        cameraRotation += cameraRotationSpeed * ts;
    else if (Input::IsKeyHeld(GLFW_KEY_E))
        cameraRotation -= cameraRotationSpeed * ts;

    camera.SetPosition(cameraPosition);
    camera.SetRotation(cameraRotation);
}

void OrthographicCameraController::OnScrollUpdate(float xOffset, float yOffset) {
    zoomLevel -= yOffset * 0.25f;
    zoomLevel = std::max(zoomLevel, 0.25f);
    camera.SetProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel);
}

void OrthographicCameraController::OnViewportResized(float width, float height) {
    aspectRatio = width / height;
    camera.SetProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel);
}

void OrthographicCameraController::OnWindowResize(int width, int height) {
    OnViewportResized((float)width, (float)height);
}