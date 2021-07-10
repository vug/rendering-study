#pragma once

#include "Timestep.h"

class ScrollListener {
public:
	virtual void OnScrollUpdate(float xOffset, float yOffset) = 0;
};