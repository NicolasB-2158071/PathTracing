#include "ConductorBxDF.h"
#include "../SurfaceIntersection.h"
#include "../../../Utility/Math.h"

ConductorBxDF::ConductorBxDF(const Json::Value& mat) :
	m_eta{ mat["eta"][0].asFloat(), mat["eta"][1].asFloat(), mat["eta"][2].asFloat() },
	m_k{ mat["k"][0].asFloat(), mat["k"][1].asFloat(), mat["k"][2].asFloat() },
	m_ggx{ mat["alphaX"].asFloat(), mat["alphaY"].asFloat() } {}

glm::vec3 ConductorBxDF::F(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const {
	if (m_ggx.IsSmooth()) { return {}; } // Probability of continous random variable is 0

	glm::vec3 localWo{ ToLocal(wo, si.normal) }, localWi{ ToLocal(wi, si.normal) };
	float cosThetaO{ glm::abs(localWo.z) }, cosThetaI{ glm::abs(wi.z) };
	if (cosThetaO == 0 || cosThetaI == 0) { return {}; }

	glm::vec3 wm{ localWi + localWo }; // Halfway vector
	if (glm::length(wm) == 0.0f) { return {}; }
	wm = glm::normalize(wm);
	return m_ggx.D(wm) * FresnelConductor(wm, localWo, m_eta, m_k) * m_ggx.G(localWo, localWi) / (4.0f * cosThetaO * cosThetaI);
}

BxDFSample ConductorBxDF::SampleF(float u, const glm::vec2& uv, const glm::vec3& wo, const SurfaceIntersection& si) const {
	if (m_ggx.IsSmooth()) {
		glm::vec3 wi{ glm::reflect(-wo, si.normal) };
		glm::vec3 f{ FresnelConductor(si.normal, wi, m_eta, m_k) / glm::abs(glm::dot(si.normal, wi)) };
		return BxDFSample{ f, wi, 1.0f, BxDFType::SPECULAR };
	}

	glm::vec3 localWo{ ToLocal(wo, si.normal) };
	glm::vec3 wm{ m_ggx.SampleEllipsoid(localWo, uv)};
	glm::vec3 wi{ glm::reflect(-localWo, wm) };

	float cosThetaO{ glm::abs(localWo.z) }, cosThetaI{ glm::abs(wi.z) };
	if (cosThetaO == 0 || cosThetaI == 0) { return {}; }

	float pdf{ m_ggx.D(localWo, wm) / (4.0f * glm::abs(glm::dot(localWo, wm))) };
	glm::vec3 f{ m_ggx.D(wm) * FresnelConductor(wm, localWo, m_eta, m_k) * m_ggx.G(localWo, wi) / (4.0f * cosThetaO * cosThetaI) };
	return BxDFSample{ f, FromLocal(wi, si.normal), pdf, BxDFType::GLOSSY };
}