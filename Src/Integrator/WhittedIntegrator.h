#pragma once

#include "Integrator.h"

class WhittedIntegrator : public Integrator {
public:
	WhittedIntegrator(Scene& scene, int maxDepth);
	glm::vec3 EvaluateSample(RTCRayHit& ray, int depth) override;
};