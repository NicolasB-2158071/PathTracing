#pragma once

#include <vector>
#include <memory>
#include "../Scene/Light/Light.h"

// Uniform light sampler
class LightSampler {
public:
	LightSampler(const std::vector<std::unique_ptr<Light>>& lights);

	const std::unique_ptr<Light>& Sample(float u) const;
	float Pmf() const; // Discrete

private:
	const std::vector<std::unique_ptr<Light>>& m_lights;
};