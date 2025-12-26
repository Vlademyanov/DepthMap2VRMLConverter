#pragma once

#include <string>
#include <glm/glm.hpp>

enum class LightingModel {
    LAMBERT,
    PHONG,
    OREN_NAYAR
};

enum class MeshFormat {
    STL,
    VRML,
    PLY
};

struct InputConfig {
    std::string depthMapFile;
};

struct OutputConfig {
    std::string meshFile;
    MeshFormat meshFormat;
    std::string imageFile;
    int imageWidth;
    int imageHeight;
};

struct LightingConfig {
    LightingModel model;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    float shininess;
    float roughness;
};

struct CameraConfig {
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float nearPlane;
    float farPlane;
};

struct RenderingConfig {
    glm::vec3 backgroundColor;
    glm::vec3 objectColor;
};

struct AppConfig {
    InputConfig input;
    OutputConfig output;
    LightingConfig lighting;
    CameraConfig camera;
    RenderingConfig rendering;
};