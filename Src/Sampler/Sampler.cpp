#include "Sampler.h"

Sampler::Sampler() : m_generator{ m_rd() } {}

float Sampler::GetUV1D() {
	return Get1D(0.0f, 1.0f);
}

float Sampler::Get1D(float min, float max) {
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(m_generator);
}

glm::vec2 Sampler::GetUV2D() {
	return Get2D(0.0f, 1.0f);
}

glm::vec2 Sampler::Get2D(float min, float max) {
	return glm::vec2{Get1D(min, max), Get1D(min, max)};
}
