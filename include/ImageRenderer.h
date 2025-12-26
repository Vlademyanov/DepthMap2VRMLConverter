#pragma once

#include "Config.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class Mesh;
class ILightingModel;

class ImageRenderer {
public:
    ImageRenderer(int width, int height);
    ~ImageRenderer();
    
    void setCamera(const CameraConfig& camera);
    void setLighting(const LightingConfig& lighting);
    void setBackgroundColor(const glm::vec3& color);
    void setObjectColor(const glm::vec3& color);
    
    void renderToImage(const Mesh& mesh, const std::string& filename);
    
private:
    int m_width;
    int m_height;
    
    std::vector<glm::vec3> m_colorBuffer;
    std::vector<float> m_depthBuffer;
    
    CameraConfig m_camera;
    LightingConfig m_lightingConfig;
    glm::vec3 m_backgroundColor;
    glm::vec3 m_objectColor;
    
    std::unique_ptr<ILightingModel> m_lightingModel;
    
    void clearBuffers();
    void rasterizeTriangle(
        const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
        const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2,
        const glm::mat4& mvp, const glm::mat4& model
    );
    
    glm::vec3 computePixelColor(
        const glm::vec3& worldPos,
        const glm::vec3& normal
    ) const;
    
    bool isInsideTriangle(float x, float y, 
        const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2) const;
    
    void createLightingModel();
};