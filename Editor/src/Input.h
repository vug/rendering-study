#pragma once

#include "Timestep.h"

class ScrollListener {
public:
	virtual void OnScrollUpdate(float xOffset, float yOffset) = 0;
};

class WindowListener {
public:
	virtual void OnWindowResize(int width, int height) = 0;
};