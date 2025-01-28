#include "PhongBRDF.h"
#include <numbers>
#include "../../../Sampler/Sampling.h"
#include "../SurfaceIntersection.h"
#include "../../../Utility/Math.h"

PhongBRDF::PhongBRDF(const Json::Value& mat) : m_rhoDiffuse{
	mat["diffuse"][0].asFloat(), mat["diffuse"][1].asFloat(),mat["diffuse"][2].asFloat() }, m_rhoSpecular{
	mat["specular"][0].asFloat(), mat["specular"][1].asFloat(), mat["specular"][2].asFloat()
	}, m_shininess{ mat["shininess"].asFloat() } {
	float d{ glm::max(m_rhoDiffuse.x, glm::max(m_rhoDiffuse.y, m_rhoDiffuse.z)) };
	float s{ glm::max(m_rhoSpecular.x, glm::max(m_rhoSpecular.y, m_rhoSpecular.z)) };
	m_pd = d / (d + s);
	m_ps = s / (d + s);
}

glm::vec3 PhongBRDF::F(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const {
	glm::vec3 refl{ glm::reflect(-wo, si.normal) };
	float alpha = glm::abs(glm::dot(wi, refl));

	glm::vec3 d{ m_rhoDiffuse * std::numbers::inv_pi_v<float> };
	glm::vec3 s{ (m_rhoSpecular * 0.5f * std::numbers::inv_pi_v<float> *glm::pow(alpha, m_shininess)) * (m_shininess + 2) };
	return d + s;
}

BxDFSample PhongBRDF::SampleF(float u, const glm::vec2& uv, const glm::vec3& wo, const SurfaceIntersection& si) const {
	if (u < m_ps) {
		glm::vec3 refl{ glm::reflect(-wo, si.normal) };
		glm::vec3 wi{ FromLocal(SampleCosineLobeHemisphere(uv, m_shininess), refl) };
		return {
			GetSpecularComponent(wo, wi, si),
			wi,
			CosineLobeHemispherePdf(glm::abs(glm::dot(wi, refl)), m_shininess) * m_ps,
			BxDFType::GLOSSY
		};
	}
	else {
		glm::vec3 wi{ FromLocal(SampleCosineHemisphere(uv), si.normal) };
		return {
			GetDiffuseComponent(),
			wi,
			CosineHemispherePdf(glm::abs(glm::dot(wi, si.normal))) * m_pd,
			BxDFType::DIFFUSE
		};
	}
}

glm::vec3 PhongBRDF::GetDiffuseComponent() const {
	return m_rhoDiffuse * std::numbers::inv_pi_v<float>;
}

glm::vec3 PhongBRDF::GetSpecularComponent(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const {
	glm::vec3 refl{ glm::reflect(-wo, si.normal) };
	float alpha = glm::abs(glm::dot(wi, refl));
	return (m_rhoSpecular * 0.5f * std::numbers::inv_pi_v<float> * glm::pow(alpha, m_shininess)) * (m_shininess + 2);
}