#pragma once

#include <glm/glm.hpp>

struct SurfaceIntersection;

struct LiSample {
	glm::vec3 l;
	glm::vec3 y;
	glm::vec3 normal;
	float pdf;
};

enum class LightType {
	POINTLIGHT,
	AREALIGHT
};

class Light {
public:
	virtual LiSample SampleLi(const SurfaceIntersection& si, const glm::vec2& u) const = 0;
	virtual glm::vec3 Le() const = 0;
	virtual LightType GetType() const = 0;
};
