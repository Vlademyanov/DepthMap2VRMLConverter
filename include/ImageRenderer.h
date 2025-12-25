#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "ILightingModel.h"
#include "Camera.h"

class Mesh;
class Config;

// Software rasterizer for rendering mesh to image
class ImageRenderer {
public:
    ImageRenderer();
    ~ImageRenderer() = default;

    bool renderToFile(
            const Mesh& mesh,
            const Config& config,
            LightingModelPtr lightingModel
    );

private:
    struct Fragment {
        glm::vec3 position;
        glm::vec3 normal;
        float depth;
    };

    std::vector<glm::vec3> m_pixels;
    std::vector<float> m_depthBuffer;
    int m_width;
    int m_height;

    void clear(const glm::vec3& color);
    void setPixel(int x, int y, const glm::vec3& color);
    glm::vec3 getPixel(int x, int y) const;

    bool isInsideTriangle(const glm::vec2& p, const glm::vec2& v0,
                          const glm::vec2& v1, const glm::vec2& v2) const;

    glm::vec3 computeBarycentric(const glm::vec2& p, const glm::vec2& v0,
                                 const glm::vec2& v1, const glm::vec2& v2) const;

    void rasterizeTriangle(
            const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2,
            const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2,
            const Camera& camera,
            const LightConfig& light,
            LightingModelPtr lightingModel,
            const glm::vec3& surfaceColor
    );
};