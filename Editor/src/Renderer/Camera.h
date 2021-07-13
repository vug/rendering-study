#pragma once

#include <glm/glm.hpp>

class Camera {
public:
	Camera(const glm::mat4& projection)
		: Projection(projection) {}

	const glm::mat4& GetProjection() const { return Projection; }

	// SetPerspective, MakeOrthographic etc.
private:
	glm::mat4 Projection;
};
