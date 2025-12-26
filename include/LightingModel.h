#pragma once

#include <glm/glm.hpp>

struct LightingParams {
    glm::vec3 lightDir;
    glm::vec3 lightColor;
    float intensity;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    float shininess;
    float roughness;
};

class ILightingModel {
public:
    virtual ~ILightingModel() = default;
    
    virtual glm::vec3 computeColor(
        const glm::vec3& position,
        const glm::vec3& normal,
        const glm::vec3& viewPos,
        const glm::vec3& baseColor,
        const LightingParams& params
    ) const = 0;
};

class LambertModel : public ILightingModel {
public:
    glm::vec3 computeColor(
        const glm::vec3& position,
        const glm::vec3& normal,
        const glm::vec3& viewPos,
        const glm::vec3& baseColor,
        const LightingParams& params
    ) const override;
};

class PhongModel : public ILightingModel {
public:
    glm::vec3 computeColor(
        const glm::vec3& position,
        const glm::vec3& normal,
        const glm::vec3& viewPos,
        const glm::vec3& baseColor,
        const LightingParams& params
    ) const override;
};

class OrenNayarModel : public ILightingModel {
public:
    glm::vec3 computeColor(
        const glm::vec3& position,
        const glm::vec3& normal,
        const glm::vec3& viewPos,
        const glm::vec3& baseColor,
        const LightingParams& params
    ) const override;
};