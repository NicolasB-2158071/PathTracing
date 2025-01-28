#pragma once

#include "Integrator.h"

class DirectIlluminationIntegrator : public Integrator {
public:
	DirectIlluminationIntegrator(Scene& scene, int maxDepth);
	glm::vec3 EvaluateSample(RTCRayHit& ray, int depth) override;
};

// Direct lighting using light importance sampling with the light chosen uniformly