#pragma once

#include <glm/vec3.hpp>
#include "Shape.h"

struct SurfaceIntersection {
	const Shape* shape = nullptr;
	glm::vec3 pos;
	glm::vec3 normal;
};