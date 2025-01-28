#include "WhittedIntegrator.h"
#include "../Scene/Shape/SurfaceIntersection.h"
#include "../Utility/Ray.h"
#include "../Utility/Math.h"

WhittedIntegrator::WhittedIntegrator(Scene& scene, int maxDepth) : Integrator(scene, maxDepth) {}

glm::vec3 WhittedIntegrator::EvaluateSample(RTCRayHit& ray, int depth) {
	glm::vec3 L{ 0.0f };
	glm::vec3 wo{ -ray.ray.dir_x, -ray.ray.dir_y, -ray.ray.dir_z };
	SurfaceIntersection si{ m_scene.GetIntersection(m_rtcScene, ray) };
	if (si.shape == nullptr || depth == 0) {
		return L;
	}
		
	for (auto& light : m_scene.GetLights()) {
		if (light->GetType() == LightType::POINTLIGHT) {
			LiSample li{ light->SampleLi(si, m_sampler.GetUV2D()) };
			if (V(si.pos, li.y)) {
				glm::vec3 wi{ li.y - si.pos };
				L += si.shape->bxdf->F(wo, wi, si) * glm::abs(glm::dot(wi, si.normal)) * li.l / li.pdf;
			}
		}
	}

	BxDFSample bs{ si.shape->bxdf->SampleF(m_sampler.GetUV1D(), m_sampler.GetUV2D(), wo, si) };
	if (bs.type == BxDFType::SPECULAR || bs.type == BxDFType::TRANSMISSION) {
		RTCRayHit newRay = CreateRayHit(si.pos, bs.wi, 0.001f);
		L += bs.f * glm::abs(glm::dot(bs.wi, si.normal)) * EvaluateSample(newRay, depth - 1) / bs.pdf;
	}

	return L;
}

// Assumes conductor or dielectric material with perfect smootheness