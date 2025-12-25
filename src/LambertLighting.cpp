#include "LambertLighting.h"
#include <glm/glm.hpp>
#include <algorithm>

LambertLighting::LambertLighting(float ambientStrength)
        : m_ambientStrength(ambientStrength) {
}

glm::vec3 LambertLighting::calculateColor(
        const glm::vec3& normal,
        const glm::vec3& lightDir,
        const glm::vec3& viewDir,
        const glm::vec3& lightColor,
        const glm::vec3& surfaceColor
) const {
    // Ambient component
    glm::vec3 ambient = m_ambientStrength * lightColor;

    // Diffuse component (Lambert's cosine law)
    glm::vec3 norm = glm::normalize(normal);
    glm::vec3 lDir = glm::normalize(lightDir);
    float diff = std::max(glm::dot(norm, lDir), 0.0f);
    glm::vec3 diffuse = diff * lightColor;

    // Combine components
    glm::vec3 result = (ambient + diffuse) * surfaceColor;

    // Clamp to [0, 1]
    return glm::clamp(result, 0.0f, 1.0f);
}