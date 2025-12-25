#include "ImageRenderer.h"
#include "Mesh.h"
#include "Config.h"
#include "BMPWriter.h"
#include <glm/gtc/matrix_transform.hpp>
#include <limits>
#include <iostream>
#include <algorithm>

ImageRenderer::ImageRenderer()
        : m_width(0)
        , m_height(0) {
}

void ImageRenderer::clear(const glm::vec3& color) {
    std::fill(m_pixels.begin(), m_pixels.end(), color);
    std::fill(m_depthBuffer.begin(), m_depthBuffer.end(),
              std::numeric_limits<float>::max());
}

void ImageRenderer::setPixel(int x, int y, const glm::vec3& color) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_pixels[y * m_width + x] = color;
    }
}

glm::vec3 ImageRenderer::getPixel(int x, int y) const {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        return m_pixels[y * m_width + x];
    }
    return glm::vec3(0.0f);
}

bool ImageRenderer::isInsideTriangle(const glm::vec2& p, const glm::vec2& v0,
                                     const glm::vec2& v1, const glm::vec2& v2) const {
    auto sign = [](const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };

    float d1 = sign(p, v0, v1);
    float d2 = sign(p, v1, v2);
    float d3 = sign(p, v2, v0);

    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(hasNeg && hasPos);
}

glm::vec3 ImageRenderer::computeBarycentric(const glm::vec2& p, const glm::vec2& v0,
                                            const glm::vec2& v1, const glm::vec2& v2) const {
    glm::vec2 v0v1 = v1 - v0;
    glm::vec2 v0v2 = v2 - v0;
    glm::vec2 v0p = p - v0;

    float d00 = glm::dot(v0v1, v0v1);
    float d01 = glm::dot(v0v1, v0v2);
    float d11 = glm::dot(v0v2, v0v2);
    float d20 = glm::dot(v0p, v0v1);
    float d21 = glm::dot(v0p, v0v2);

    float denom = d00 * d11 - d01 * d01;
    if (std::abs(denom) < 1e-8f) {
        return glm::vec3(1.0f, 0.0f, 0.0f);
    }

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return glm::vec3(u, v, w);
}

void ImageRenderer::rasterizeTriangle(
        const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2,
        const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2,
        const Camera& camera,
        const LightConfig& light,
        LightingModelPtr lightingModel,
        const glm::vec3& surfaceColor
) {
    // Perspective divide
    glm::vec3 p0 = glm::vec3(v0) / v0.w;
    glm::vec3 p1 = glm::vec3(v1) / v1.w;
    glm::vec3 p2 = glm::vec3(v2) / v2.w;

    // NDC to screen space
    glm::vec2 s0((p0.x + 1.0f) * 0.5f * m_width, (p0.y + 1.0f) * 0.5f * m_height);
    glm::vec2 s1((p1.x + 1.0f) * 0.5f * m_width, (p1.y + 1.0f) * 0.5f * m_height);
    glm::vec2 s2((p2.x + 1.0f) * 0.5f * m_width, (p2.y + 1.0f) * 0.5f * m_height);

    // Bounding box
    float minX = std::max(0.0f, std::min({s0.x, s1.x, s2.x}));
    float maxX = std::min(static_cast<float>(m_width - 1), std::max({s0.x, s1.x, s2.x}));
    float minY = std::max(0.0f, std::min({s0.y, s1.y, s2.y}));
    float maxY = std::min(static_cast<float>(m_height - 1), std::max({s0.y, s1.y, s2.y}));

    // Rasterize
    for (int y = static_cast<int>(minY); y <= static_cast<int>(maxY); y++) {
        for (int x = static_cast<int>(minX); x <= static_cast<int>(maxX); x++) {
            glm::vec2 p(x + 0.5f, y + 0.5f);

            if (isInsideTriangle(p, s0, s1, s2)) {
                glm::vec3 bary = computeBarycentric(p, s0, s1, s2);

                // Interpolate depth
                float depth = bary.x * p0.z + bary.y * p1.z + bary.z * p2.z;

                int idx = y * m_width + x;
                if (depth < m_depthBuffer[idx]) {
                    m_depthBuffer[idx] = depth;

                    // Interpolate normal
                    glm::vec3 normal = glm::normalize(bary.x * n0 + bary.y * n1 + bary.z * n2);

                    // Interpolate position in world space
                    glm::vec3 worldPos = bary.x * glm::vec3(v0) + bary.y * glm::vec3(v1) +
                                         bary.z * glm::vec3(v2);

                    // Calculate view direction
                    glm::vec3 viewDir = glm::normalize(camera.getPosition() - worldPos);

                    // Calculate lighting
                    glm::vec3 lightDir = -light.direction;
                    glm::vec3 color = lightingModel->calculateColor(
                            normal, lightDir, viewDir,
                            light.color * light.intensity, surfaceColor
                    );

                    setPixel(x, y, color);
                }
            }
        }
    }
}

bool ImageRenderer::renderToFile(const Mesh& mesh, const Config& config,
                                 LightingModelPtr lightingModel) {
    const auto& imgConfig = config.getRenderImageConfig();

    m_width = imgConfig.width;
    m_height = imgConfig.height;
    m_pixels.resize(m_width * m_height);
    m_depthBuffer.resize(m_width * m_height);

    clear(glm::vec3(0.1f, 0.1f, 0.15f)); // Background color

    Camera camera(config.getCameraConfig());
    const LightConfig& light = config.getLightConfig();

    // Setup matrices
    float aspectRatio = static_cast<float>(m_width) / static_cast<float>(m_height);
    glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 mvp = projection * view * model;

    const auto& vertices = mesh.getVertices();
    const auto& triangles = mesh.getTriangles();

    std::cout << "Rendering " << triangles.size() << " triangles..." << std::endl;

    // Render each triangle
    for (size_t i = 0; i < triangles.size(); i++) {
        const auto& tri = triangles[i];

        const Vertex& vert0 = vertices[tri.indices[0]];
        const Vertex& vert1 = vertices[tri.indices[1]];
        const Vertex& vert2 = vertices[tri.indices[2]];

        glm::vec4 v0 = mvp * glm::vec4(vert0.position, 1.0f);
        glm::vec4 v1 = mvp * glm::vec4(vert1.position, 1.0f);
        glm::vec4 v2 = mvp * glm::vec4(vert2.position, 1.0f);

        // Simple backface culling
        glm::vec3 p0 = glm::vec3(v0) / v0.w;
        glm::vec3 p1 = glm::vec3(v1) / v1.w;
        glm::vec3 p2 = glm::vec3(v2) / v2.w;

        glm::vec3 edge1 = p1 - p0;
        glm::vec3 edge2 = p2 - p0;
        glm::vec3 faceNormal = glm::cross(edge1, edge2);

        if (faceNormal.z > 0) { // Front-facing
            rasterizeTriangle(
                    v0, v1, v2,
                    vert0.normal, vert1.normal, vert2.normal,
                    camera, light, lightingModel,
                    config.getDiffuseColor()
            );
        }

        // Progress indicator
        if ((i + 1) % 1000 == 0 || i == triangles.size() - 1) {
            std::cout << "  Progress: " << (i + 1) << "/" << triangles.size()
                      << " triangles" << std::endl;
        }
    }

    // Write to BMP file
    return BMPWriter::write(imgConfig.outputFile, m_width, m_height, m_pixels);
}