#include "ClampToneMap.h"

void ClampToneMap::Map(std::vector<float>& pixels) {
    for (int i = 0; i < pixels.size(); ++i) {
        pixels[i] = glm::clamp(pixels[i], 0.0f, 1.0f);
    }
}
