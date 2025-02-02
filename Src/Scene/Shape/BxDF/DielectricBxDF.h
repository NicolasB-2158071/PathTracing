#pragma once

#include "Bxdf.h"
#include "../../../Sampler/GGX.h"

class DielectricBxDF : public BxDF {
public:
	DielectricBxDF(const Json::Value& mat);
	glm::vec3 F(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const override;
	BxDFSample SampleF(float u, const glm::vec2& uv, const glm::vec3& wo, const SurfaceIntersection& si) const override;
	float PDF(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const override;

private:
	float m_eta;
	GGX m_ggx;
};