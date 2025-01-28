#pragma once

#include <vector>
#include <glm/glm.hpp>


class ToneMap {
public:
	virtual void Map(std::vector<float>& pixels) = 0;
};