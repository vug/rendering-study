#pragma once

#include <string>
#include "GLFW/glfw3.h"

// Basically this is a float with some helper functions.
class Timestep {
public:
	Timestep(float time = 0.0f) : time(time) {}

	operator float() const { return time; }

	float GetSeconds() const { return time; }
	float GetMilliseconds() const { return time * 1000.0f; }
private:
	float time;
};

class Application {
public:
	int Run();
	bool IsKeyHeld(int key);
protected:
	Application(std::string name);
private:
	virtual void OnInit() = 0;
	virtual void OnUpdate(Timestep ts) = 0;
	virtual void OnImGuiRender() = 0;
	virtual void OnShutdown() = 0;
	float lastFrameTime = 0.0f;
protected:
	GLFWwindow* window = nullptr;
	std::string name = "Application";
	float framesPerSecond = -1.0f;
};
