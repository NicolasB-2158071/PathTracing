#pragma once

#include "Bxdf.h"
#include "../../../Sampler/GGX.h"

class ConductorBxDF : public BxDF {
public:
	ConductorBxDF(const Json::Value& mat);
	glm::vec3 F(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const override;
	BxDFSample SampleF(float u, const glm::vec2& uv, const glm::vec3& wo, const SurfaceIntersection& si) const override;

private:
	glm::vec3 m_eta;
	glm::vec3 m_k;
	GGX m_ggx;
};

// https://chris.hindefjord.se/resources/rgb-ior-metals/