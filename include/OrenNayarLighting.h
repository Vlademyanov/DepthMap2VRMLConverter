#pragma once

#include "ILightingModel.h"

// Oren-Nayar (rough surface) lighting model
class OrenNayarLighting : public ILightingModel {
public:
    OrenNayarLighting(float roughness = 0.5f, float ambientStrength = 0.3f);
    virtual ~OrenNayarLighting() = default;

    virtual glm::vec3 calculateColor(
            const glm::vec3& normal,
            const glm::vec3& lightDir,
            const glm::vec3& viewDir,
            const glm::vec3& lightColor,
            const glm::vec3& surfaceColor
    ) const override;

    virtual std::string getName() const override { return "Oren-Nayar"; }

    void setRoughness(float roughness);

private:
    float m_roughness;      // Surface roughness parameter (0 = smooth, 1 = rough)
    float m_ambientStrength;
    float m_A;              // Precomputed coefficient A
    float m_B;              // Precomputed coefficient B

    void computeCoefficients();
};