#include "GGX.h"
#include <numbers>
#include "Sampling.h"
#include "../Utility/Math.h"

GGX::GGX(float alphaX, float alphaY) : m_alphaX{ alphaX }, m_alphaY{ alphaY } {}

float GGX::D(const glm::vec3& wm) const {
    float cos2Theta{ wm.z * wm.z };
    float sin2Theta{ glm::max(0.0f, 1.0f - cos2Theta) };
    float sinTheta{ glm::sqrt(sin2Theta) };
    float tan2Theta{ sin2Theta / cos2Theta };
    float cos4Theta{ cos2Theta * cos2Theta };

    float cosPhi{ sinTheta == 0.0f ? 1.0f : glm::clamp(wm.x / sinTheta, -1.0f, 1.0f) };
    float sinPhi{ sinTheta == 0.0f ? 1.0f : glm::clamp(wm.y / sinTheta, -1.0f, 1.0f) };
    float e{ tan2Theta * ((cosPhi * cosPhi) / (m_alphaX * m_alphaX) + (sinPhi * sinPhi) / (m_alphaY * m_alphaY)) };
    return 1.0f / (std::numbers::pi_v<float> * m_alphaX * m_alphaY * cos4Theta * (1.0f + e) * (1.0f + e));
}

float GGX::D(const glm::vec3& wo, const glm::vec3& wm) const {
    return G1(wo) / glm::abs(wo.z) * D(wm) * glm::abs(glm::dot(wo, wm));
}

float GGX::G(const glm::vec3& wo, const glm::vec3& wi) const {
    return 1.0f / (1.0f + Lambda(wo) + Lambda(wi));
}

float GGX::G1(const glm::vec3& wo) const {
    return 1.0f / (1.0f + Lambda(wo));
}

float GGX::Lambda(const glm::vec3& w) const {
    float cos2Theta{ w.z * w.z };
    float sin2Theta{ glm::max(0.0f, 1.0f - cos2Theta) };
    float sinTheta{ glm::sqrt(sin2Theta) };
    float tan2Theta{ sin2Theta / cos2Theta };

    float cosPhi{ sinTheta == 0.0f ? 1.0f : glm::clamp(w.x / sinTheta, -1.0f, 1.0f) };
    float sinPhi{ sinTheta == 0.0f ? 1.0f : glm::clamp(w.y / sinTheta, -1.0f, 1.0f) };
    float alpha2{ (cosPhi * m_alphaX) * (cosPhi * m_alphaX) + (sinPhi * m_alphaY) * (sinPhi * m_alphaY) };
    return (glm::sqrt(1.0f + alpha2 * tan2Theta) - 1.0f) / 2.0f;
}

bool GGX::IsSmooth() const {
    return glm::max(m_alphaX, m_alphaY) < 1e-3f;;
}

glm::vec3 GGX::SampleEllipsoid(const glm::vec3& wo, const glm::vec2& uv) const {
    // Convert ellipsoid to hemisphere
    glm::vec3 wh{ glm::normalize(glm::vec3(m_alphaY * wo.x, m_alphaY * wo.y, wo.z)) };
    if (wh.z < 0) { wh = -wh; }

    // Find orthonormal basis around wh
    glm::vec3 T1{ (wh.z < 0.99999f)
        ? glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), wh))
        : glm::vec3(1.0f, 0.0f, 0.0f)
    };
    glm::vec3 T2{ glm::cross(wh, T1) };

    // Find point (uniformly) on unit disk
    glm::vec2 p{ SampleUniformDiskPolar(uv) };

    // Project point on hemisphere
    float h{ std::sqrt(1.0f - p.x * p.x) };
    p.y = Lerp((1.0f + wh.z) / 2.0f, h, p.y);

    // Reproject back to ellipsoid
    float pz{ std::sqrt(std::max<float>(0.0f, 1.0f - glm::length(p) * glm::length(p))) };
    glm::vec3 nh{ p.x * T1 + p.y * T2 + pz * wh };
    return glm::normalize(glm::vec3(m_alphaX * nh.x, m_alphaY * nh.y, std::max<float>(1e-6f, nh.z)));
}
