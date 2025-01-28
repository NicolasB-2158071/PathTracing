#pragma once

#include "Bxdf.h"

class LambertianDiffuseBRDF : public BxDF {
public:
	LambertianDiffuseBRDF(const Json::Value& mat);
	glm::vec3 F(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const override;
	BxDFSample SampleF(float u, const glm::vec2& uv, const glm::vec3& wo, const SurfaceIntersection& si) const override;

private:
	glm::vec3 m_rho;
};

