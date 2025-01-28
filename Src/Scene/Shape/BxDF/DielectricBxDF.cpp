#include "DielectricBxDF.h"
#include "../SurfaceIntersection.h"
#include "../../../Utility/Math.h"

DielectricBxDF::DielectricBxDF(const Json::Value& mat) :
	m_eta{ mat["eta"].asFloat() },
	m_ggx{ mat["alphaX"].asFloat(), mat["alphaY"].asFloat() } {}

glm::vec3 DielectricBxDF::F(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const {
	if (m_ggx.IsSmooth()) { return {}; } // Probability of continous random variable is 0

	glm::vec3 localWo{ ToLocal(wo, si.normal) }, localWi{ ToLocal(wi, si.normal) };
	float cosThetaO{ glm::abs(localWo.z) }, cosThetaI{ glm::abs(wi.z) };
	if (cosThetaO == 0 || cosThetaI == 0) { return {}; }

	bool reflect{ cosThetaO * cosThetaI > 0.0f };
	float etap{ 1.0f }; // If reflection -> no changes calculation halfway vector
	if (!reflect) {
		etap = cosThetaO < 0.0f ? (1.0f / m_eta) : m_eta;
	}
	glm::vec3 wm{ localWi * etap + localWo };
	if (glm::length(wm) == 0.0f) { return {}; }
	wm = glm::normalize(wm);
	wm = wm.z < 0.0f ? -wm : wm; // Local space

	if (glm::dot(wm, localWi) * cosThetaI < 0.0f || glm::dot(wm, localWo) * cosThetaO < 0.0f) { return {}; }

	float fr{ FresnelDielectric(wm, localWo, m_eta) }, tr{ 1.0f - fr };
	if (reflect) {
		return glm::vec3{ m_ggx.D(wm) * fr * m_ggx.G(localWo, localWi) / (4.0f * cosThetaO * cosThetaI) };
	} else {
		float denom{ glm::dot(localWi, wm) + glm::dot(localWo, wm) / etap }; denom *= denom;
		float absCosWiWm{ glm::abs(glm::dot(localWi, wm)) };
		float absCosWoWm{ glm::abs(glm::dot(localWo, wm)) };
		return glm::vec3{ m_ggx.D(wm) * tr * m_ggx.G(localWo, localWi) * absCosWiWm * absCosWoWm / (cosThetaO * cosThetaI * denom) };
	}
}

BxDFSample DielectricBxDF::SampleF(float u, const glm::vec2& uv, const glm::vec3& wo, const SurfaceIntersection& si) const {
	if (m_ggx.IsSmooth()) {
		float fr{ FresnelDielectric(si.normal, wo, m_eta) }, tr{ 1.0f - fr };
		if (u < fr / (fr + tr)) {
			glm::vec3 wi{ glm::reflect(-wo, si.normal) };
			glm::vec3 f{ fr / glm::abs(glm::dot(si.normal, wi)) };
			return BxDFSample{ f, wi, fr / (fr + tr), BxDFType::SPECULAR };
		}
		else {
			std::optional<glm::vec3> wi{ Refract(si.normal, wo, m_eta) };
			if (!wi.has_value()) { return{}; }
			glm::vec3 f{ tr / glm::abs(glm::dot(si.normal, wi.value())) };
			return BxDFSample{ f, wi.value(), tr / (fr + tr), BxDFType::TRANSMISSION };
		}
	}

	glm::vec3 localWo{ ToLocal(wo, si.normal) };
	glm::vec3 wm{ m_ggx.SampleEllipsoid(localWo, uv) };
	float fr{ FresnelDielectric(wm, localWo, m_eta) }, tr{ 1.0f - fr };
	if (u < fr / (fr + tr)) {
		glm::vec3 wi{ glm::reflect(-localWo, wm) };
		float cosThetaO{ glm::abs(localWo.z) }, cosThetaI{ glm::abs(wi.z) };
		if (cosThetaO == 0 || cosThetaI == 0) { return {}; }

		float pdf{ m_ggx.D(localWo, wm) / (4.0f * glm::abs(glm::dot(localWo, wm))) * fr / (fr + tr) };
		glm::vec3 f{ m_ggx.D(wm) * fr * m_ggx.G(localWo, wi) / (4.0f * cosThetaO * cosThetaI) };
		return BxDFSample{ f, FromLocal(wi, si.normal), pdf, BxDFType::GLOSSY };
	} else {
		std::optional<glm::vec3> wi{ Refract(wm, localWo, m_eta) };
		if (!wi.has_value()) { return{}; }

		float cosThetaO{ glm::abs(localWo.z) }, cosThetaI{ glm::abs(wi.value().z) };
		float etap{ glm::dot(wm, localWo) < 0.0f ? 1.0f / m_eta : m_eta };

		float denom{ glm::dot(wi.value(), wm) + glm::dot(localWo, wm) / etap }; denom *= denom;
		float absCosWiWm{ glm::abs(glm::dot(wi.value(), wm)) };
		float absCosWoWm{ glm::abs(glm::dot(localWo, wm)) };
		float pdf{ m_ggx.D(localWo, wm) * absCosWiWm / denom * tr / (fr + tr) };

		glm::vec3 f{ m_ggx.D(wm) * tr * m_ggx.G(localWo, wi.value()) * absCosWiWm * absCosWoWm / (cosThetaO * cosThetaI * denom) };
		return BxDFSample{ f, FromLocal(wi.value(), si.normal), pdf, BxDFType::GLOSSYTRANSMISSION };
	}

}