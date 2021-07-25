#pragma once

class KeyListener {
public:
	virtual void OnKeyPress(int key, int action, int mods) = 0;
};

class ScrollListener {
public:
	virtual void OnScrollUpdate(float xOffset, float yOffset) = 0;
};

class MouseButtonListener {
public:
	virtual void OnMouseButtonClicked(int button, int action, int mods) = 0;
};

class WindowListener {
public:
	virtual void OnWindowResize(int width, int height) = 0;
};
