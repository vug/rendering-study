#pragma once

#include "Timestep.h"
#include "Application.h"

class Input {
public:
	static bool IsKeyHeld(int key) {
		int keyState = glfwGetKey(Application::Get().GetWindow(), key);
		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
	}

	static float GetMouseX() {
		double xPos, yPos;
		glfwGetCursorPos(Application::Get().GetWindow(), &xPos, &yPos);
		return (float) xPos;
	}

	static float GetMouseY() {
		double xPos, yPos;
		glfwGetCursorPos(Application::Get().GetWindow(), &xPos, &yPos);
		return (float)yPos;
	}

	static bool IsMouseButtonPressed(int button) {
		int buttonState = glfwGetMouseButton(Application::Get().GetWindow(), button);
		return buttonState == GLFW_PRESS;
	}
};



