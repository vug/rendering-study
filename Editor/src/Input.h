#pragma once

#include "Timestep.h"
#include "Application.h"

class Input {
public:
	static bool IsKeyHeld(int key) {
		int keyState = glfwGetKey(Application::Get().GetWindow(), key);
		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
	}
};



