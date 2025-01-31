#pragma once

#include "Light.h"

class Shape;

class AreaLight : public Light {
public:
	AreaLight(const Shape& shape, const glm::vec3& radiance, bool twoSided);

	LiSample SampleLi(const SurfaceIntersection& si, const glm::vec2& u) const override; // Uniform
	float LiPDF(const SurfaceIntersection& si, const SurfaceIntersection& prevSi) const override;
	glm::vec3 Le() const override;
	LightType GetType() const override;

private:
	const Shape& m_shape;
	glm::vec3 m_radiance;
	bool m_twoSided;
};