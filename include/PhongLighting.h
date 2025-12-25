#pragma once

#include "ILightingModel.h"

// Phong (diffuse + specular) lighting model
class PhongLighting : public ILightingModel {
public:
    PhongLighting(
            float ambientStrength = 0.3f,
            float specularStrength = 0.5f,
            float shininess = 32.0f
    );
    virtual ~PhongLighting() = default;

    virtual glm::vec3 calculateColor(
            const glm::vec3& normal,
            const glm::vec3& lightDir,
            const glm::vec3& viewDir,
            const glm::vec3& lightColor,
            const glm::vec3& surfaceColor
    ) const override;

    virtual std::string getName() const override { return "Phong"; }

    void setShininess(float shininess) { m_shininess = shininess; }
    void setSpecularColor(const glm::vec3& color) { m_specularColor = color; }

private:
    float m_ambientStrength;
    float m_specularStrength;
    float m_shininess;
    glm::vec3 m_specularColor;
};