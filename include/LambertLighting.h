#pragma once

#include "ILightingModel.h"

// Lambert (diffuse only) lighting model
class LambertLighting : public ILightingModel {
public:
    LambertLighting(float ambientStrength = 0.3f);
    virtual ~LambertLighting() = default;

    virtual glm::vec3 calculateColor(
            const glm::vec3& normal,
            const glm::vec3& lightDir,
            const glm::vec3& viewDir,
            const glm::vec3& lightColor,
            const glm::vec3& surfaceColor
    ) const override;

    virtual std::string getName() const override { return "Lambert"; }

private:
    float m_ambientStrength;
};