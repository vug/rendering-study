#pragma once

#include <memory>
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
	void RegisterKeyListener(KeyListener* listener);
	void RegisterMouseButtonListener(MouseButtonListener* listener);
	void RegisterScrollListener(ScrollListener* listener);
	void RegisterWindowListener(WindowListener* listener);
	const bool& GetIsViewportPaneFocused() { return isViewportPaneFocused; }
	const bool& GetIsViewportPaneHovered() { return isViewportPaneHovered; }
protected:
	GLFWwindow* window = nullptr;
	std::string name = "Application";
	float framesPerSecond = -1.0f;
	glm::vec2 viewportBounds[2];
	std::shared_ptr<Framebuffer> viewportFramebuffer;
private:
	// To prepare rendering entities
	virtual void OnInit() = 0;
	// Will be called when viewport pane is resized
	virtual void OnViewportResize(float width, float height) = 0;
	// Rendering commands and user interaction will be written into this method
	virtual void OnUpdate(Timestep ts) = 0;
	virtual void OnImGuiRender() = 0;
	virtual void OnImGuiViewportRender() = 0;
	virtual void OnShutdown() = 0;

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void windowSizeCallback(GLFWwindow* window, int width, int height);
private:
	static Application* instance;
	float lastFrameTime = 0.0f;
	std::vector<KeyListener*> keyListeners;
	std::vector<MouseButtonListener*> mouseButtonListeners;
	std::vector<ScrollListener*> scrollListeners;
	std::vector<WindowListener*> windowListeners;
	glm::vec2 viewportSize = { 0.0f, 0.0f };
	bool isViewportPaneFocused = false;
	bool isViewportPaneHovered = false;
};
