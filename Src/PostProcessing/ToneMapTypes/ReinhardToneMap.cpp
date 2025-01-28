#include "ReinhardToneMap.h"

void ReinhardToneMap::Map(std::vector<float>& pixels) {
    float whitePoint = std::max(CalculateWhitePoint(pixels), 1e-6f);
    ApplyExtendedReinhardToneMap(pixels, whitePoint);
}

float ReinhardToneMap::CalculateWhitePoint(std::vector<float>& pixels) const {
    float maxLuminance = 10.0f;
    for (int i = 0; i < pixels.size(); i += 3) {
        float luminance = 0.2126f * pixels[i] + 0.7152f * pixels[i + 1] + 0.0722f * pixels[i + 2];
        maxLuminance = std::max(maxLuminance, luminance);
    }
    return maxLuminance;
}

void ReinhardToneMap::ApplyExtendedReinhardToneMap(std::vector<float>& pixels, float whitePoint) {
    for (int i = 0; i < pixels.size(); i += 3) {
        glm::vec3 color = glm::clamp(ExtendedReinhardTonemap(glm::vec3{pixels[i], pixels[i + 1], pixels[i + 2]}, whitePoint), 0.0f, 1.0f);
        pixels[i] = color.r;
        pixels[i + 1] = color.g;
        pixels[i + 2] = color.b;
    }
}

glm::vec3 ReinhardToneMap::ExtendedReinhardTonemap(const glm::vec3& color, float whitePoint) {
    glm::vec3 scaledColor = color * (1.0f + color / (whitePoint * whitePoint));
    return scaledColor / (1.0f + color);
}
