#include "Integrator.h"
#include "../Utility/Ray.h"
#include "../Utility/Math.h"
#include "WhittedIntegrator.h"
#include "RandomWalkIntegrator.h"
#include "DirectIlluminationIntegrator.h"
#include "IndirectIlluminationIntegrator.h"
#include <numbers>

Integrator::Integrator(Scene& scene, int maxDepth) : m_rtcScene{ nullptr }, m_scene{ scene }, m_sampler{}, m_lightSampler{ m_scene.GetLights() }, m_maxDepth{ maxDepth } {}

Integrator::~Integrator() {
	rtcReleaseScene(m_rtcScene);
}

void Integrator::InitRTCScene(RTCDevice device) {
	m_rtcScene = m_scene.Build(device);
}

const Scene& Integrator::GetScene() const {
	return m_scene;
}

int Integrator::GetMaxDepth() const {
	return m_maxDepth;
}

std::unique_ptr<Integrator> Integrator::CreateFromName(const std::string& name, Scene& scene, int maxDepth) {
	if (name == "WhittedIntegrator") {
		return std::make_unique<WhittedIntegrator>(scene, maxDepth);
	}
	if (name == "RandomWalkIntegrator") {
		return std::make_unique<RandomWalkIntegrator>(scene, maxDepth);
	}
	if (name == "DirectIlluminationIntegrator") {
		return std::make_unique<DirectIlluminationIntegrator>(scene, maxDepth);
	}
	if (name == "IndirectIlluminationIntegrator") {
		return std::make_unique<IndirectIlluminationIntegrator>(scene, maxDepth);
	}
}

bool Integrator::V(const glm::vec3& x, const glm::vec3& y) const {
	RTCRayHit shadowRay{ CreateRayHit(x, glm::normalize(y - x), 0.001f, glm::distance(x, y) - 0.001f) };
	RTCOccludedArguments sargs;
	rtcInitOccludedArguments(&sargs);
	rtcOccluded1(m_rtcScene, &shadowRay.ray, &sargs);
	return shadowRay.ray.tfar >= 0.0f;
}

float Integrator::G(const glm::vec3& x, const glm::vec3& nx, const glm::vec3& y, const glm::vec3& ny) const {
	float cosx{ glm::dot(nx, glm::normalize(y - x)) };
	float cosy{ glm::dot(ny, glm::normalize(x - y)) };
	float dist{ glm::distance(x, y) };
	return cosx * cosy / (dist * dist);
}

