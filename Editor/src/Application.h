#pragma once

#include <string>
#include "GLFW/glfw3.h"

class Application {
public:
	int Run();
protected:
	Application(std::string name);
private:
	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnImGuiRender() = 0;
	virtual void OnShutdown() = 0;
protected:
	GLFWwindow* window = nullptr;
	std::string name = "Application";
};
