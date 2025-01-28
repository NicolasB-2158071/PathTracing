#pragma once

#include <glm/glm.hpp>
#include <numbers>

// Using Malley's method
inline glm::vec3 SampleCosineHemisphere(const glm::vec2& uv) {
    float phi{ 2.0f * std::numbers::pi_v<float> * uv.x };
    float theta{ std::acos(std::sqrt(1.0f - uv.y)) };
    float x{ std::cos(phi) * std::sin(theta) }, y{ std::sin(phi) * std::sin(theta) }, z{ std::cos(theta) };
    return glm::vec3{ x, y, z };
}

inline float CosineHemispherePdf(float cosTheta) {
    return cosTheta * std::numbers::inv_pi_v<float>;
}

inline glm::vec3 SampleCosineLobeHemisphere(const glm::vec2& uv, float n) {
    float phi{ 2.0f * std::numbers::pi_v<float> * uv.x }, theta{ glm::pow(uv.y, 2.0f / (n + 1.0f)) };
    float x{ std::cos(phi) * std::sqrt(1.0f - theta) }, y{ std::sin(phi) * std::sqrt(1.0f - theta) }, z{ glm::pow(uv.y, 1.0f / (n + 1.0f)) };
    return glm::vec3{ x, y, z };
}

inline float CosineLobeHemispherePdf(float cosAlpha, float n) {
    return (n + 1.0f) * glm::pow(cosAlpha, n) * 0.5f * std::numbers::inv_pi_v<float>;
}

inline glm::vec3 SampleUniformHemisphere(const glm::vec2& uv) {
    float z{ 1.0f - 2.0f * uv.x }, r{ std::sqrt(1.0f - z * z) }, phi{ 2.0f * std::numbers::pi_v<float> * uv.y };
    float x{ r * std::cos(phi) }, y{ r * std::sin(phi) };
    return glm::vec3{ x, y, z };
}

inline float UniformHemispherePdf() {
    return 0.25f * std::numbers::inv_pi_v<float>;
}

inline glm::vec2 SampleUniformDiskPolar(const glm::vec2& uv) {
    float r{ std::sqrt(uv.x) };
    float theta{ 2.0f * std::numbers::pi_v<float> * uv.y };
    return glm::vec2{ r * std::cos(theta), r * std::sin(theta) };
}

inline float PowerHeuristic(float fPdf, float gPdf, int nf = 1, int ng = 1) {
    float f{ nf * fPdf }, g{ ng * gPdf };
    return f * f / (f * f + g * g);
}