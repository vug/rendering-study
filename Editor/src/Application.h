#pragma once

#include <string>
#include <vector>

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>

#include "Timestep.h"
#include "Listener.h"
#include "Renderer/Framebuffer.h"

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
	const bool& GetIsViewportPaneFocused() { return isViewportPaneFocused; }
	const bool& GetIsViewportPaneHovered() { return isViewportPaneHovered; }
protected:
	GLFWwindow* window = nullptr;
	std::string name = "Application";
	float framesPerSecond = -1.0f;
private:
	// To prepare rendering entities
	virtual void OnInit() = 0;
	// Will be called when viewport pane is resized
	virtual void OnViewportResize(float width, float height) = 0;
	// Rendering commands and user interaction will be written into this method
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
	std::shared_ptr<Framebuffer> viewportFramebuffer;
	glm::vec2 viewportSize = { 0.0f, 0.0f };
	bool isViewportPaneFocused = false;
	bool isViewportPaneHovered = false;
};
