#include "LightingModel.h"
#include <algorithm>
#include <cmath>

// Модель Ламберта (только диффузное освещение)
glm::vec3 LambertModel::computeColor(
    const glm::vec3& position,
    const glm::vec3& normal,
    const glm::vec3& viewPos,
    const glm::vec3& baseColor,
    const LightingParams& params
) const {
    glm::vec3 norm = glm::normalize(normal);
    glm::vec3 lightDir = glm::normalize(-params.lightDir);
    
    // Ambient
    glm::vec3 ambient = params.ambientStrength * params.lightColor;
    
    // Diffuse (Lambert)
    float diff = std::max(glm::dot(norm, lightDir), 0.0f);
    glm::vec3 diffuse = params.diffuseStrength * diff * params.lightColor;
    
    glm::vec3 result = (ambient + diffuse) * baseColor * params.intensity;
    return glm::clamp(result, 0.0f, 1.0f);
}

// Модель Фонга (ambient + diffuse + specular)
glm::vec3 PhongModel::computeColor(
    const glm::vec3& position,
    const glm::vec3& normal,
    const glm::vec3& viewPos,
    const glm::vec3& baseColor,
    const LightingParams& params
) const {
    glm::vec3 norm = glm::normalize(normal);
    glm::vec3 lightDir = glm::normalize(-params.lightDir);
    glm::vec3 viewDir = glm::normalize(viewPos - position);
    
    // Ambient
    glm::vec3 ambient = params.ambientStrength * params.lightColor;
    
    // Diffuse
    float diff = std::max(glm::dot(norm, lightDir), 0.0f);
    glm::vec3 diffuse = params.diffuseStrength * diff * params.lightColor;
    
    // Specular (Phong)
    glm::vec3 reflectDir = glm::reflect(-lightDir, norm);
    float spec = std::pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), params.shininess);
    glm::vec3 specular = params.specularStrength * spec * params.lightColor;
    
    glm::vec3 result = (ambient + diffuse + specular) * baseColor * params.intensity;
    return glm::clamp(result, 0.0f, 1.0f);
}

// Модель Орена-Найара (для шероховатых поверхностей)
glm::vec3 OrenNayarModel::computeColor(
    const glm::vec3& position,
    const glm::vec3& normal,
    const glm::vec3& viewPos,
    const glm::vec3& baseColor,
    const LightingParams& params
) const {
    glm::vec3 norm = glm::normalize(normal);
    glm::vec3 lightDir = glm::normalize(-params.lightDir);
    glm::vec3 viewDir = glm::normalize(viewPos - position);
    
    // Ambient
    glm::vec3 ambient = params.ambientStrength * params.lightColor;
    
    // Oren-Nayar diffuse
    float roughness = params.roughness;
    float sigma2 = roughness * roughness;
    
    float A = 1.0f - 0.5f * sigma2 / (sigma2 + 0.33f);
    float B = 0.45f * sigma2 / (sigma2 + 0.09f);
    
    float NdotL = glm::dot(norm, lightDir);
    float NdotV = glm::dot(norm, viewDir);
    
    float angleVN = std::acos(glm::clamp(NdotV, -1.0f, 1.0f));
    float angleLN = std::acos(glm::clamp(NdotL, -1.0f, 1.0f));
    
    float alpha = std::max(angleVN, angleLN);
    float beta = std::min(angleVN, angleLN);
    
    glm::vec3 lightProj = glm::normalize(lightDir - norm * NdotL);
    glm::vec3 viewProj = glm::normalize(viewDir - norm * NdotV);
    float cosPhi = glm::clamp(glm::dot(lightProj, viewProj), 0.0f, 1.0f);
    
    float L = std::max(NdotL, 0.0f) * (A + B * cosPhi * std::sin(alpha) * std::tan(beta));
    
    glm::vec3 diffuse = params.diffuseStrength * L * params.lightColor;
    
    // Simple specular for Oren-Nayar
    glm::vec3 halfDir = glm::normalize(lightDir + viewDir);
    float spec = std::pow(std::max(glm::dot(norm, halfDir), 0.0f), params.shininess);
    glm::vec3 specular = params.specularStrength * spec * params.lightColor;
    
    glm::vec3 result = (ambient + diffuse + specular) * baseColor * params.intensity;
    return glm::clamp(result, 0.0f, 1.0f);
}