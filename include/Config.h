#pragma once

#include <string>
#include <glm/glm.hpp>

enum class LightingModel {
    LAMBERT,
    PHONG,
    OREN_NAYAR
};

enum class ExportFormat {
    VRML,
    STL,
    PLY,
    OBJ
};

enum class ProjectionType {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

struct CameraConfig {
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float nearPlane;
    float farPlane;
    ProjectionType projectionType;
};

struct LightConfig {
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
};

struct RenderImageConfig {
    int width;
    int height;
    std::string outputFile;
    bool enabled;
};

class Config {
public:
    Config();
    ~Config();

    bool loadFromFile(const std::string& filename);

    // Getters
    const std::string& getInputFile() const { return m_inputFile; }
    const std::string& getOutputFile() const { return m_outputFile; }
    ExportFormat getExportFormat() const { return m_exportFormat; }
    LightingModel getLightingModel() const { return m_lightingModel; }
    const CameraConfig& getCameraConfig() const { return m_cameraConfig; }
    const LightConfig& getLightConfig() const { return m_lightConfig; }
    const RenderImageConfig& getRenderImageConfig() const { return m_renderImageConfig; }

    bool isVisualizationEnabled() const { return m_enableVisualization; }

    // Surface material properties
    float getRoughness() const { return m_roughness; }
    glm::vec3 getDiffuseColor() const { return m_diffuseColor; }
    glm::vec3 getSpecularColor() const { return m_specularColor; }
    float getShininess() const { return m_shininess; }

private:
    std::string m_inputFile;
    std::string m_outputFile;
    ExportFormat m_exportFormat;
    LightingModel m_lightingModel;
    CameraConfig m_cameraConfig;
    LightConfig m_lightConfig;
    RenderImageConfig m_renderImageConfig;
    bool m_enableVisualization;

    // Material properties
    float m_roughness;
    glm::vec3 m_diffuseColor;
    glm::vec3 m_specularColor;
    float m_shininess;

    void setDefaults();
    LightingModel parseLightingModel(const std::string& model);
    ExportFormat parseExportFormat(const std::string& format);
    ProjectionType parseProjectionType(const std::string& type);
};