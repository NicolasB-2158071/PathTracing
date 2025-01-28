#pragma once

#include <glm/glm.hpp>
#include <complex>
#include <optional>

struct OrthonormalBasis {
    glm::vec3 e1;
    glm::vec3 e2;
    glm::vec3 e3;
};

// https://github.com/alexanderameye/rayrs/blob/master/rayrs-lib/src/vecmath.rs#L341
inline OrthonormalBasis OrthonormalBasisFromNormal(const glm::vec3& normal) {
    glm::vec3 e1{ glm::abs(normal.x) > glm::abs(normal.y) ?
        glm::normalize(glm::vec3{normal.z, 0.0f, -normal.x}) :
        glm::normalize(glm::vec3{0.0f, normal.z, -normal.y})
    };
    return { e1, glm::normalize(glm::cross(normal, e1)), normal };
}

inline glm::vec3 ToLocal(const glm::vec3& v, const glm::vec3& normal) {
    OrthonormalBasis basis{ OrthonormalBasisFromNormal(normal) };
    return glm::vec3{ glm::dot(v, basis.e1), glm::dot(v, basis.e2), glm::dot(v, basis.e3) }; // Project to find components
}

inline glm::vec3 FromLocal(const glm::vec3& v, const glm::vec3& normal) {
    OrthonormalBasis basis{ OrthonormalBasisFromNormal(normal) };
    return v.x * basis.e1 + v.y * basis.e2 + v.z * basis.e3;
}

inline float Lerp(float x, float a, float b) {
    return (1.0f - x) * a + x * b;
}

inline glm::vec3 SchlickApproximation(const glm::vec3& f0, float theta) {
    return f0 + (glm::vec3{ 1.0f } - f0) * static_cast<float>(glm::pow(1.0f - theta, 5));
}

// Taken from:
// https://pbr-book.org/4ed/Reflection_Models/Specular_Reflection_and_Transmission
inline std::optional<glm::vec3> Refract(const glm::vec3& normal, const glm::vec3& wi, float eta) {
    float cosThetaI{ glm::dot(normal, wi) };
    if (cosThetaI < 0.0f) {
        cosThetaI = -cosThetaI;
        eta = 1.0f / eta;
    }

    float sin2ThetaI{ glm::max(0.0f, 1.0f - cosThetaI * cosThetaI) };
    float sin2ThetaT = sin2ThetaI / (eta * eta);
    if (sin2ThetaT >= 1.0f) {
        return std::nullopt;
    }
    float cosThetaT = sqrtf(1.0f - sin2ThetaT);
    return std::optional<glm::vec3>{-wi / eta + (cosThetaI / eta - cosThetaT) * (cosThetaI < 0.0f ? -normal : normal)};

}

inline float FresnelDielectric(const glm::vec3& normal, const glm::vec3& wi, float eta) {
    float cosThetaI{ glm::clamp(glm::dot(wi, normal), -1.0f, 1.0f) };
    if (cosThetaI < 0.0f) { // From inside to outside (dielectric: transmission possible)
        eta = 1.0f / eta;
        cosThetaI = -cosThetaI;
    }

    // Calculate cosTheta of transmission ray
    float sin2ThetaI{ 1.0f - cosThetaI * cosThetaI };
    float sin2ThetaT = sin2ThetaI / (eta * eta);
    if (sin2ThetaT >= 1.0f) { // From less to more optical dense: behaves like ideal reflection instead of transmission depending on angle
        return 1.0f;
    }
    float cosThetaT = sqrtf(1.0f - sin2ThetaT);

    float rParl = (eta * cosThetaI - cosThetaT) / (eta * cosThetaI + cosThetaT);
    float rPerp = (cosThetaI - eta * cosThetaT) / (cosThetaI + eta * cosThetaT);
    return (rParl * rParl + rPerp * rPerp) / 2.0f;
}

// eta = eta, k
inline float FresnelConductor(const glm::vec3& normal, const glm::vec3& wi, const std::complex<float>& eta) { // k "models the decay of light as it travels deeper into the material", effects reflection
    float cosThetaI{ glm::dot(normal, wi) };

    // Calculate cosTheta of transmission ray
    float sin2ThetaI{ 1.0f - cosThetaI * cosThetaI };
    std::complex<float> sin2ThetaT = sin2ThetaI / (eta * eta);
    std::complex<float> cosThetaT = sqrt(1.0f - sin2ThetaT);

    std::complex<float> rParl = (eta * cosThetaI - cosThetaT) / (eta * cosThetaI + cosThetaT);
    std::complex<float> rPerp = (cosThetaI - eta * cosThetaT) / (cosThetaI + eta * cosThetaT);
    return (std::abs(rParl) * std::abs(rParl) + std::abs(rPerp) * std::abs(rPerp)) / 2.0f;
}

inline glm::vec3 FresnelConductor(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& eta, const glm::vec3& k) {
    glm::vec3 result;
    for (int i = 0; i < 3; ++i) {
        result[i] = FresnelConductor(normal, wi, std::complex<float>{eta[i], k[i]});
    }
    return result;
}

// Wavelength depended ior for dielectrics is done differently (pbrt: samples wavelengths)