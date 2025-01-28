#pragma once

#include "Integrator.h"

class RandomWalkIntegrator : public Integrator {
public:
	RandomWalkIntegrator(Scene& scene, int maxDepth);
	glm::vec3 EvaluateSample(RTCRayHit& ray, int depth) override;
};