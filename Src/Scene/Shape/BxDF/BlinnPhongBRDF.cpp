#include "BlinnPhongBRDF.h"
#include <numbers>
#include "../../../Sampler/Sampling.h"
#include "../../../Utility/Math.h"
#include "../SurfaceIntersection.h"

BlinnPhongBRDF::BlinnPhongBRDF(const Json::Value& mat) : m_rhoDiffuse{
	mat["diffuse"][0].asFloat(), mat["diffuse"][1].asFloat(),mat["diffuse"][2].asFloat() }, m_rhoSpecular{
	mat["specular"][0].asFloat(), mat["specular"][1].asFloat(), mat["specular"][2].asFloat()
	}, m_shininess{ mat["shininess"].asFloat() } {
	float d{ glm::max(m_rhoDiffuse.x, glm::max(m_rhoDiffuse.y, m_rhoDiffuse.z)) };
	float s{ glm::max(m_rhoSpecular.x, glm::max(m_rhoSpecular.y, m_rhoSpecular.z)) };
	m_pd = d / (d + s);
	m_ps = s / (d + s);
}

glm::vec3 BlinnPhongBRDF::F(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const {
	glm::vec3 h{ glm::normalize(wo + wi) };
	float alpha = glm::abs(glm::dot(h, si.normal));

	glm::vec3 d{ m_rhoDiffuse * std::numbers::inv_pi_v<float> };
	glm::vec3 s{ (m_rhoSpecular * 0.5f * std::numbers::inv_pi_v<float> * glm::pow(alpha, m_shininess)) * (m_shininess + 2) };
	return d + s;
}

BxDFSample BlinnPhongBRDF::SampleF(float u, const glm::vec2& uv, const glm::vec3& wo, const SurfaceIntersection& si) const {
	if (u < m_ps) {
		glm::vec3 h{ FromLocal(SampleCosineLobeHemisphere(uv, m_shininess), si.normal) };
		glm::vec3 wi{ glm::reflect(-wo, h)}; // if h is constant, you get a clear highlight
		return {
			GetSpecularComponent(wo, wi, si),
			wi,
			CosineLobeHemispherePdf(glm::abs(glm::dot(h, si.normal)), m_shininess) * m_ps,
			BxDFType::GLOSSY
		};
	} else {
		glm::vec3 wi{ FromLocal(SampleCosineHemisphere(uv), si.normal) };
		return {
			GetDiffuseComponent(),
			wi,
			CosineHemispherePdf(glm::abs(glm::dot(wi, si.normal))) * m_pd,
			BxDFType::DIFFUSE
		};
	}
}

glm::vec3 BlinnPhongBRDF::GetDiffuseComponent() const {
	return m_rhoDiffuse * std::numbers::inv_pi_v<float>;
}

glm::vec3 BlinnPhongBRDF::GetSpecularComponent(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const {
	glm::vec3 h{ glm::normalize(wo + wi) };
	float alpha = glm::abs(glm::dot(h, si.normal));
	return (m_rhoSpecular * 0.5f * std::numbers::inv_pi_v<float> * glm::pow(alpha, m_shininess)) * (m_shininess + 2);
}