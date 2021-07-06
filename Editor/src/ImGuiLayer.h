#pragma once

class ImGuiLayer {
public:
	static void Init(GLFWwindow* window);
	static void Begin();
	static void End();
	static void Shutdown();
};
