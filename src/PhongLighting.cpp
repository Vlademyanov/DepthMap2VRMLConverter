#include "PhongLighting.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>

PhongLighting::PhongLighting(float ambientStrength, float specularStrength, float shininess)
        : m_ambientStrength(ambientStrength)
        , m_specularStrength(specularStrength)
        , m_shininess(shininess)
        , m_specularColor(1.0f) {
}

glm::vec3 PhongLighting::calculateColor(
        const glm::vec3& normal,
        const glm::vec3& lightDir,
        const glm::vec3& viewDir,
        const glm::vec3& lightColor,
        const glm::vec3& surfaceColor
) const {
    // Ambient component
    glm::vec3 ambient = m_ambientStrength * lightColor;

    // Diffuse component
    glm::vec3 norm = glm::normalize(normal);
    glm::vec3 lDir = glm::normalize(lightDir);
    float diff = std::max(glm::dot(norm, lDir), 0.0f);
    glm::vec3 diffuse = diff * lightColor;

    // Specular component (Phong reflection model)
    glm::vec3 vDir = glm::normalize(viewDir);
    glm::vec3 reflectDir = glm::reflect(-lDir, norm);
    float spec = std::pow(std::max(glm::dot(vDir, reflectDir), 0.0f), m_shininess);
    glm::vec3 specular = m_specularStrength * spec * lightColor * m_specularColor;

    // Combine components
    glm::vec3 result = (ambient + diffuse) * surfaceColor + specular;

    // Clamp to [0, 1]
    return glm::clamp(result, 0.0f, 1.0f);
}