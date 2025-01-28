#pragma once

#include "Integrator.h"

class IndirectIlluminationIntegrator : public Integrator {
public:
	IndirectIlluminationIntegrator(Scene& scene, int maxDepth);
	glm::vec3 EvaluateSample(RTCRayHit& ray, int depth) override;
};