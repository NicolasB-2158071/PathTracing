#include "LightSampler.h"

LightSampler::LightSampler(const std::vector<std::unique_ptr<Light>>& lights) : m_lights{ lights } {}

const std::unique_ptr<Light>& LightSampler::Sample(float u) const {
	return m_lights[std::min<int>(u * m_lights.size(), m_lights.size() - 1)];
}

float LightSampler::Pmf() const {
	return 1.0f / m_lights.size();
}
