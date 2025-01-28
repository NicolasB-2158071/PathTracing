#pragma once
#include "../ToneMap.h"
#include "../../Image/Image.h"

class ClampToneMap : public ToneMap {
public:
	void Map(std::vector<float>& pixels) override;
};