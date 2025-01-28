#pragma once

#include "../ToneMap.h"


class ReinhardToneMap : public ToneMap {
public:
	void Map(std::vector<float>& pixels) override;

private:
	float CalculateWhitePoint(std::vector<float>& pixels) const;
	void ApplyExtendedReinhardToneMap(std::vector<float>& pixels, float whitePoint);
	glm::vec3 ExtendedReinhardTonemap(const glm::vec3 &color, float whitePoint);
};
