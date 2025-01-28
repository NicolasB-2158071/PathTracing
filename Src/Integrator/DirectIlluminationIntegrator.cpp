#include "DirectIlluminationIntegrator.h"
#include "../Scene/Shape/SurfaceIntersection.h"

DirectIlluminationIntegrator::DirectIlluminationIntegrator(Scene& scene, int maxDepth) : Integrator(scene, maxDepth) {}

glm::vec3 DirectIlluminationIntegrator::EvaluateSample(RTCRayHit& ray, int depth) {
	glm::vec3 L{ 0.0f };
	glm::vec3 wo{ -ray.ray.dir_x, -ray.ray.dir_y, -ray.ray.dir_z };
	SurfaceIntersection si{ m_scene.GetIntersection(m_rtcScene, ray) };
	if (si.shape == nullptr) {
		return L;
	}

	if (si.shape->areaLight != nullptr) {
		L += si.shape->areaLight->Le();
	} else {
		LiSample li{ m_lightSampler.Sample(m_sampler.GetUV1D())->SampleLi(si, m_sampler.GetUV2D()) };
		if (V(si.pos, li.y)) {
			float g = G(si.pos, si.normal, li.y, li.normal);
			L += si.shape->bxdf->F(wo, glm::normalize(li.y - si.pos), si) * g * li.l / (li.pdf * m_lightSampler.Pmf());
		}
	}
	return L;
}
