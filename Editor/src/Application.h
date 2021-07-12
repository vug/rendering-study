#pragma once

#include <string>
#include <vector>

#include "GLFW/glfw3.h"

#include "Timestep.h"
#include "Listener.h"

class Application {
public:
	int Run();

	static Application& Get() { return *instance; }
	GLFWwindow* GetWindow() { return window; }
protected:
	Application(std::string name);
	void Close();
	void RegisterScrollListener(ScrollListener* listener);
	void RegisterWindowListener(WindowListener* listener);
protected:
	GLFWwindow* window = nullptr;
	std::string name = "Application";
	float framesPerSecond = -1.0f;
private:
	virtual void OnInit() = 0;
	virtual void OnUpdate(Timestep ts) = 0;
	virtual void OnImGuiRender() = 0;
	virtual void OnShutdown() = 0;

	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void windowSizeCallback(GLFWwindow* window, int width, int height);
private:
	static Application* instance;
	float lastFrameTime = 0.0f;
	std::vector<ScrollListener*> scrollListeners;
	std::vector<WindowListener*> windowListeners;
};
