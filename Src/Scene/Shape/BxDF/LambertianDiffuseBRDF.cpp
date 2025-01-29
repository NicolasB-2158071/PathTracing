#include "LambertianDiffuseBRDF.h"
#include <numbers>
#include "../../../Sampler/Sampling.h"
#include "../SurfaceIntersection.h"
#include "../../../Utility/Math.h"

LambertianDiffuseBRDF::LambertianDiffuseBRDF(const Json::Value& mat) : m_rho{
	mat["diffuse"][0].asFloat(), mat["diffuse"][1].asFloat(), mat["diffuse"][2].asFloat() 
} {}

glm::vec3 LambertianDiffuseBRDF::F(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const {
	return m_rho * std::numbers::inv_pi_v<float>;
}

BxDFSample LambertianDiffuseBRDF::SampleF(float u, const glm::vec2& uv, const glm::vec3& wo, const SurfaceIntersection& si) const {
	glm::vec3 wi{ FromLocal(SampleCosineHemisphere(uv), si.normal) };
	return {
		F(wo, wi, si),
		wi,
		CosineHemispherePdf(glm::abs(glm::dot(wi, si.normal))),
		BxDFType::DIFFUSE
	};
}

float LambertianDiffuseBRDF::PDF(const glm::vec3& wo, const glm::vec3& wi, const SurfaceIntersection& si) const {
	return CosineHemispherePdf(glm::abs(glm::dot(wi, si.normal)));
}
