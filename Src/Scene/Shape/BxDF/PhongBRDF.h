#pragma once

#include "Bxdf.h"
#include "tiny_obj_loader.h"

class PhongBRDF : public BxDF {
public:
	PhongBRDF(const Json::Value& mat);
	glm::vec3 F(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const override;
	BxDFSample SampleF(float u, const glm::vec2& uv, const glm::vec3& wo, const SurfaceIntersection& si) const override;

private:
	glm::vec3 m_rhoDiffuse;
	glm::vec3 m_rhoSpecular;
	float m_shininess;
	float m_ps;
	float m_pd;

	glm::vec3 GetDiffuseComponent() const;
	glm::vec3 GetSpecularComponent(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const;
};

// Modified Phong cf. Phong Eric P. Lafortune, Yves DWillems