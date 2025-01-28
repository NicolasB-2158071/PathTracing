#include "RandomWalkIntegrator.h"
#include "../Scene/Shape/SurfaceIntersection.h"
#include "../Utility/Ray.h"
#include "../Sampler/Sampling.h"
#include "../Utility/Math.h"

RandomWalkIntegrator::RandomWalkIntegrator(Scene& scene, int maxDepth) : Integrator(scene, maxDepth) {}

glm::vec3 RandomWalkIntegrator::EvaluateSample(RTCRayHit& ray, int depth) {
	glm::vec3 L{ 0.0f }, beta{ 1.0f };
	while (beta != glm::vec3{ 0.0f }) {
		glm::vec3 wo{ -ray.ray.dir_x, -ray.ray.dir_y, -ray.ray.dir_z };
		SurfaceIntersection si{ m_scene.GetIntersection(m_rtcScene, ray) };
		if (si.shape == nullptr || depth-- == 0) {
			break;
		}
		if (si.shape->areaLight != nullptr) {
			L += beta * si.shape->areaLight->Le();
			continue; // Skip if it's an emitter
		}
	
		glm::vec3 wi{ FromLocal(SampleUniformHemisphere(m_sampler.GetUV2D()), si.normal)};
		beta *= si.shape->bxdf->F(wo, wi, si) * glm::abs(glm::dot(wi, si.normal)) / UniformHemispherePdf();
		ray = CreateRayHit(si.pos, wi, 0.001f);
	}

	return L;
}
