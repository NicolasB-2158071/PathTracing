#include "IndirectIlluminationIntegrator.h"
#include "../Scene/Shape/SurfaceIntersection.h"
#include "../Utility/Ray.h"
#include "../Sampler/Sampling.h"

IndirectIlluminationIntegrator::IndirectIlluminationIntegrator(Scene& scene, int maxDepth) : Integrator(scene, maxDepth) {}

glm::vec3 IndirectIlluminationIntegrator::EvaluateSample(RTCRayHit& ray, int depth) {
	glm::vec3 L{ 0.0f }, beta{ 1.0f };
	float pL{}, pB{};
	//SurfaceIntersection prev{};
	while (beta != glm::vec3{ 0.0f }) {
		glm::vec3 wo{ -ray.ray.dir_x, -ray.ray.dir_y, -ray.ray.dir_z };
		SurfaceIntersection si{ m_scene.GetIntersection(m_rtcScene, ray) };
		if (si.shape == nullptr || depth-- == 0) {
			break;
		}
		if (si.shape->areaLight != nullptr) {
			if (depth + 1 == m_maxDepth) { // Initial camera ray: "light sampling was not performed at the previous vertex of the path"
				L += beta * si.shape->areaLight->Le();
				break; // Skip if it's an emitter
			}
			//float wB{ PowerHeuristic(pB, m_lightSampler.Pmf() * si.shape->areaLight->LiPDF(si, prev)) };
			//L += wB * beta * si.shape->areaLight->Le();
		}

		LiSample li{ m_lightSampler.Sample(m_sampler.GetUV1D())->SampleLi(si, m_sampler.GetUV2D()) };
		if (V(si.pos, li.y)) {
			glm::vec3 wi{ glm::normalize(li.y - si.pos) };
			float g{ G(si.pos, si.normal, li.y, li.normal) };
			L += beta * si.shape->bxdf->F(wo, wi, si) * g * li.l / (li.pdf * m_lightSampler.Pmf());
			//float dist{ glm::distance(si.pos, li.y) };
			//li.pdf /= glm::abs(glm::dot(li.normal, -wi)) / (dist * dist);
			//pL = li.pdf * m_lightSampler.Pmf();
			//float wL{ PowerHeuristic(pL, si.shape->bxdf->PDF(wo, wi, si)) };
			//L += wL * beta * si.shape->bxdf->F(wo, wi, si) * li.l * glm::abs(glm::dot(wi, si.normal)) / pL;
		}
		BxDFSample bs{ si.shape->bxdf->SampleF(m_sampler.GetUV1D(), m_sampler.GetUV2D(), wo, si) };
		beta *= bs.f * glm::abs(glm::dot(bs.wi, si.normal)) / bs.pdf;
		//pB = bs.pdf;

		float maxComponent{ glm::max(beta.x, glm::max(beta.y, beta.z)) };
		float q{ glm::max(0.0f, 1.0f - maxComponent) };
		if (m_sampler.GetUV1D() < q) {
			break;
		}
		beta /= 1.0f - q;
		ray = CreateRayHit(si.pos, bs.wi, 0.001f);
		//prev = si;
	}

	return L;
}