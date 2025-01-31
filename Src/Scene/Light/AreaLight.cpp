#include "AreaLight.h"
#include "../Shape/Shape.h"
#include "../Shape/SurfaceIntersection.h"

AreaLight::AreaLight(const Shape& shape, const glm::vec3& radiance, bool twoSided) : m_shape{ shape }, m_radiance{ radiance }, m_twoSided{ twoSided } {}

LiSample AreaLight::SampleLi(const SurfaceIntersection& si, const glm::vec2& u) const {
	int faceID{ std::min<int>(u.x * m_shape.GetNumFaces(), m_shape.GetNumFaces() - 1) };
	glm::vec3 y{ m_shape.SampleUniform(faceID, u) };
	
	LiSample li{ Le(), y, glm::vec3{0.0f}, 1.0f / m_shape.GetArea() };
	glm::vec2 uv{ m_shape.GetBarycentric(y, faceID) };
	rtcInterpolate1(m_shape.rtcGeometry, faceID, uv.x, uv.y, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &li.normal.x, nullptr, nullptr, 3);
	li.l = glm::dot(li.normal, glm::normalize(si.pos - li.y)) < 0.0f ? glm::vec3{ 0.0f } : li.l;
	return li;
}

float AreaLight::LiPDF(const SurfaceIntersection& si, const SurfaceIntersection& prevSi) const {
	glm::vec3 wi{ glm::normalize(prevSi.pos - si.pos) };
	float dist{ glm::length(wi) };
	return (1.0f / m_shape.GetArea()) / (glm::abs(glm::dot(si.normal, -wi) / (dist * dist)));
}

glm::vec3 AreaLight::Le() const {
	return m_radiance / ((m_twoSided ? 2.0f : 1.0f) * m_shape.GetArea());
}

LightType AreaLight::GetType() const {
	return LightType::AREALIGHT;
}
