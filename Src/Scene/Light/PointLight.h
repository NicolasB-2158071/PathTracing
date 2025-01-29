#pragma once

#include "Light.h"

class PointLight : public Light {
public:
	PointLight(const glm::vec3& pos, const glm::vec3& radiance);

	LiSample SampleLi(const SurfaceIntersection& si, const glm::vec2& u) const override;
	float LiPDF() const override;
	glm::vec3 Le() const override;
	LightType GetType() const override;

private:
	glm::vec3 m_pos;
	glm::vec3 m_radiance;
};
