#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <string>

// Interface for lighting models (Strategy pattern)
class ILightingModel {
public:
    virtual ~ILightingModel() = default;

    // Calculate color based on lighting model
    // Parameters:
    // - normal: surface normal at the point
    // - lightDir: direction to light source (normalized)
    // - viewDir: direction to viewer (normalized)
    // - lightColor: color of the light
    // - surfaceColor: base color of the surface
    // Returns: final color after lighting calculation
    virtual glm::vec3 calculateColor(
            const glm::vec3& normal,
            const glm::vec3& lightDir,
            const glm::vec3& viewDir,
            const glm::vec3& lightColor,
            const glm::vec3& surfaceColor
    ) const = 0;

    // Get model name for debugging/logging
    virtual std::string getName() const = 0;
};

using LightingModelPtr = std::shared_ptr<ILightingModel>;