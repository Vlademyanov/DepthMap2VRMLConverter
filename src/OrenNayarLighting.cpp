#include "OrenNayarLighting.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>

OrenNayarLighting::OrenNayarLighting(float roughness, float ambientStrength)
        : m_roughness(roughness)
        , m_ambientStrength(ambientStrength)
        , m_A(0.0f)
        , m_B(0.0f) {
    computeCoefficients();
}

void OrenNayarLighting::setRoughness(float roughness) {
    m_roughness = roughness;
    computeCoefficients();
}

void OrenNayarLighting::computeCoefficients() {
    float sigma2 = m_roughness * m_roughness;
    m_A = 1.0f - 0.5f * sigma2 / (sigma2 + 0.33f);
    m_B = 0.45f * sigma2 / (sigma2 + 0.09f);
}

glm::vec3 OrenNayarLighting::calculateColor(
        const glm::vec3& normal,
        const glm::vec3& lightDir,
        const glm::vec3& viewDir,
        const glm::vec3& lightColor,
        const glm::vec3& surfaceColor
) const {
    // Ambient component
    glm::vec3 ambient = m_ambientStrength * lightColor;

    // Normalize vectors
    glm::vec3 N = glm::normalize(normal);
    glm::vec3 L = glm::normalize(lightDir);
    glm::vec3 V = glm::normalize(viewDir);

    // Calculate angles
    float NdotL = glm::dot(N, L);
    float NdotV = glm::dot(N, V);

    // Oren-Nayar requires positive dot products
    if (NdotL <= 0.0f) {
        return ambient * surfaceColor;
    }

    float thetaI = std::acos(glm::clamp(NdotL, 0.0f, 1.0f));
    float thetaR = std::acos(glm::clamp(NdotV, 0.0f, 1.0f));

    float alpha = std::max(thetaI, thetaR);
    float beta = std::min(thetaI, thetaR);

    // Calculate azimuthal difference
    glm::vec3 lightProjected = glm::normalize(L - N * NdotL);
    glm::vec3 viewProjected = glm::normalize(V - N * NdotV);
    float cosPhi = glm::dot(lightProjected, viewProjected);

    // Oren-Nayar reflectance
    float C1 = 1.0f - 0.5f * m_roughness * m_roughness / (m_roughness * m_roughness + 0.33f);
    float C2 = 0.45f * m_roughness * m_roughness / (m_roughness * m_roughness + 0.09f);

    if (cosPhi >= 0.0f) {
        C2 *= std::sin(alpha);
    } else {
        C2 *= (std::sin(alpha) - std::pow(2.0f * beta / glm::pi<float>(), 3.0f));
    }

    float C3 = 0.125f * m_roughness * m_roughness / (m_roughness * m_roughness + 0.09f);
    C3 *= std::pow((4.0f * alpha * beta) / (glm::pi<float>() * glm::pi<float>()), 2.0f);

    float L1 = NdotL * (m_A + m_B * std::max(0.0f, cosPhi) * std::sin(alpha) * std::tan(beta));

    // Combine components
    glm::vec3 diffuse = L1 * lightColor;
    glm::vec3 result = (ambient + diffuse) * surfaceColor;

    // Clamp to [0, 1]
    return glm::clamp(result, 0.0f, 1.0f);
}