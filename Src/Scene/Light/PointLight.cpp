#include "PointLight.h"
#include "../Shape/SurfaceIntersection.h"

PointLight::PointLight(const glm::vec3& pos, const glm::vec3& radiance) : m_pos{ pos }, m_radiance{ radiance } {}

LiSample PointLight::SampleLi(const SurfaceIntersection& si, const glm::vec2& u) const {
	return LiSample{Le(), m_pos, glm::normalize(m_pos - si.pos), 1.0f};
}

float PointLight::LiPDF() const {
	return 1.0f;
}

glm::vec3 PointLight::Le() const {
	return m_radiance;
}

LightType PointLight::GetType() const {
	return LightType::POINTLIGHT;
}