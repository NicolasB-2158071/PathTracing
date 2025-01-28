#pragma once

#include <random>
#include <glm/vec2.hpp>

class Sampler {
public:
	Sampler();

	float GetUV1D();
	float Get1D(float min, float max);

	glm::vec2 GetUV2D();
	glm::vec2 Get2D(float min, float max);

private:
	std::random_device m_rd;
	std::mt19937 m_generator;
};