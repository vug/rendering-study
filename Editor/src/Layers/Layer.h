#pragma once

#include "../Timestep.h"

class Layer {
public:
	Layer() = default;
	Layer(Layer& layer) = default;
	virtual void OnInit() = 0;
	virtual void OnUpdate(Timestep ts) = 0;
};