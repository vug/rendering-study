#include "OrthographicCameraController.h"

OrthographicCameraController::OrthographicCameraController(Application& app, float aspectRatio)
    : app(app), aspectRatio(aspectRatio), camera(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel) {
}

void OrthographicCameraController::OnUpdate(Timestep ts) {
    if (app.IsKeyHeld(GLFW_KEY_A)) 
        cameraPosition.x -= cameraTranslationSpeed * ts;
    else if (app.IsKeyHeld(GLFW_KEY_D))
        cameraPosition.x += cameraTranslationSpeed * ts;

    if (app.IsKeyHeld(GLFW_KEY_W)) 
        cameraPosition.y += cameraTranslationSpeed * ts;
    else if (app.IsKeyHeld(GLFW_KEY_S)) 
        cameraPosition.y -= cameraTranslationSpeed * ts;

    if (app.IsKeyHeld(GLFW_KEY_Q)) 
        cameraRotation += cameraRotationSpeed * ts;
    else if (app.IsKeyHeld(GLFW_KEY_E)) 
        cameraRotation -= cameraRotationSpeed * ts;

    camera.SetPosition(cameraPosition);
    camera.SetRotation(cameraRotation);
}


bool OrthographicCameraController::OnMouseScrolled() {
    // zoomLevel -= scrollAmount
    camera.SetProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel);
    return false;
}

bool OrthographicCameraController::OnWindowResized() {
    // change aspect ratio here?
    // aspectRatio = (float)app.GetWindows().GetWidth() / (float)app.GetWindows().GetHeight();
    camera.SetProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel);
    return false;
}