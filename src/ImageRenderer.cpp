#include "ImageRenderer.h"
#include "Mesh.h"
#include "LightingModel.h"
#include "BMPWriter.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>

ImageRenderer::ImageRenderer(int width, int height)
    : m_width(width)
    , m_height(height) {
    m_colorBuffer.resize(width * height);
    m_depthBuffer.resize(width * height);
}

ImageRenderer::~ImageRenderer() = default;

void ImageRenderer::setCamera(const CameraConfig& camera) {
    m_camera = camera;
}

void ImageRenderer::setLighting(const LightingConfig& lighting) {
    m_lightingConfig = lighting;
    createLightingModel();
}

void ImageRenderer::setBackgroundColor(const glm::vec3& color) {
    m_backgroundColor = color;
}

void ImageRenderer::setObjectColor(const glm::vec3& color) {
    m_objectColor = color;
}

void ImageRenderer::createLightingModel() {
    switch (m_lightingConfig.model) {
        case LightingModel::LAMBERT:
            m_lightingModel = std::make_unique<LambertModel>();
            break;
        case LightingModel::PHONG:
            m_lightingModel = std::make_unique<PhongModel>();
            break;
        case LightingModel::OREN_NAYAR:
            m_lightingModel = std::make_unique<OrenNayarModel>();
            break;
    }
}

void ImageRenderer::clearBuffers() {
    std::fill(m_colorBuffer.begin(), m_colorBuffer.end(), m_backgroundColor);
    std::fill(m_depthBuffer.begin(), m_depthBuffer.end(), std::numeric_limits<float>::max());
}

glm::vec3 ImageRenderer::computePixelColor(
    const glm::vec3& worldPos,
    const glm::vec3& normal
) const {
    if (!m_lightingModel) {
        return m_objectColor;
    }
    
    LightingParams params;
    params.lightDir = m_lightingConfig.direction;
    params.lightColor = m_lightingConfig.color;
    params.intensity = m_lightingConfig.intensity;
    params.ambientStrength = m_lightingConfig.ambientStrength;
    params.diffuseStrength = m_lightingConfig.diffuseStrength;
    params.specularStrength = m_lightingConfig.specularStrength;
    params.shininess = m_lightingConfig.shininess;
    params.roughness = m_lightingConfig.roughness;
    
    return m_lightingModel->computeColor(
        worldPos, normal, m_camera.position, m_objectColor, params
    );
}

bool ImageRenderer::isInsideTriangle(float x, float y,
    const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2) const {
    
    auto sign = [](const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };
    
    glm::vec2 p(x, y);
    float d1 = sign(p, v0, v1);
    float d2 = sign(p, v1, v2);
    float d3 = sign(p, v2, v0);
    
    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    
    return !(hasNeg && hasPos);
}

void ImageRenderer::rasterizeTriangle(
    const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
    const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2,
    const glm::mat4& mvp, const glm::mat4& model
) {
    // Transform to clip space
    glm::vec4 clip0 = mvp * glm::vec4(v0, 1.0f);
    glm::vec4 clip1 = mvp * glm::vec4(v1, 1.0f);
    glm::vec4 clip2 = mvp * glm::vec4(v2, 1.0f);
    
    // Perspective divide
    glm::vec3 ndc0 = glm::vec3(clip0) / clip0.w;
    glm::vec3 ndc1 = glm::vec3(clip1) / clip1.w;
    glm::vec3 ndc2 = glm::vec3(clip2) / clip2.w;
    
    // Transform to screen space
    glm::vec2 screen0((ndc0.x + 1.0f) * 0.5f * m_width, (1.0f - ndc0.y) * 0.5f * m_height);
    glm::vec2 screen1((ndc1.x + 1.0f) * 0.5f * m_width, (1.0f - ndc1.y) * 0.5f * m_height);
    glm::vec2 screen2((ndc2.x + 1.0f) * 0.5f * m_width, (1.0f - ndc2.y) * 0.5f * m_height);
    
    // Bounding box
    int minX = std::max(0, static_cast<int>(std::floor(std::min({screen0.x, screen1.x, screen2.x}))));
    int maxX = std::min(m_width - 1, static_cast<int>(std::ceil(std::max({screen0.x, screen1.x, screen2.x}))));
    int minY = std::max(0, static_cast<int>(std::floor(std::min({screen0.y, screen1.y, screen2.y}))));
    int maxY = std::min(m_height - 1, static_cast<int>(std::ceil(std::max({screen0.y, screen1.y, screen2.y}))));
    
    // Rasterize
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            if (!isInsideTriangle(x + 0.5f, y + 0.5f, screen0, screen1, screen2)) {
                continue;
            }
            
            // Barycentric coordinates
            glm::vec2 p(x + 0.5f, y + 0.5f);
            float area = ((screen1.y - screen2.y) * (screen0.x - screen2.x) + 
                         (screen2.x - screen1.x) * (screen0.y - screen2.y));
            
            if (std::abs(area) < 1e-6f) continue;
            
            float w0 = ((screen1.y - screen2.y) * (p.x - screen2.x) + 
                       (screen2.x - screen1.x) * (p.y - screen2.y)) / area;
            float w1 = ((screen2.y - screen0.y) * (p.x - screen2.x) + 
                       (screen0.x - screen2.x) * (p.y - screen2.y)) / area;
            float w2 = 1.0f - w0 - w1;
            
            // Interpolate depth
            float depth = w0 * ndc0.z + w1 * ndc1.z + w2 * ndc2.z;
            
            int pixelIndex = y * m_width + x;
            if (depth < m_depthBuffer[pixelIndex]) {
                m_depthBuffer[pixelIndex] = depth;
                
                // Interpolate world position and normal
                glm::vec3 worldPos = w0 * v0 + w1 * v1 + w2 * v2;
                glm::vec3 normal = glm::normalize(w0 * n0 + w1 * n1 + w2 * n2);
                
                // Transform normal to world space
                glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
                normal = glm::normalize(normalMatrix * normal);
                
                // Compute color
                m_colorBuffer[pixelIndex] = computePixelColor(worldPos, normal);
            }
        }
    }
}

void ImageRenderer::renderToImage(const Mesh& mesh, const std::string& filename) {
    std::cout << "Rendering to image..." << std::endl;
    
    clearBuffers();
    
    const auto& vertices = mesh.getVertices();
    const auto& triangles = mesh.getTriangles();
    
    // Setup matrices
    float aspect = static_cast<float>(m_width) / static_cast<float>(m_height);
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(m_camera.position, m_camera.target, m_camera.up);
    glm::mat4 projection = glm::perspective(
        glm::radians(m_camera.fov),
        aspect,
        m_camera.nearPlane,
        m_camera.farPlane
    );
    
    glm::mat4 mvp = projection * view * model;
    
    // Rasterize all triangles
    int triangleCount = 0;
    for (const auto& tri : triangles) {
        const Vertex& v0 = vertices[tri.indices[0]];
        const Vertex& v1 = vertices[tri.indices[1]];
        const Vertex& v2 = vertices[tri.indices[2]];
        
        rasterizeTriangle(
            v0.position, v1.position, v2.position,
            v0.normal, v1.normal, v2.normal,
            mvp, model
        );
        
        triangleCount++;
        if (triangleCount % 1000 == 0) {
            std::cout << "  Processed " << triangleCount << "/" << triangles.size() 
                     << " triangles" << std::endl;
        }
    }
    
    std::cout << "Writing BMP file..." << std::endl;
    BMPWriter::write(filename, m_width, m_height, m_colorBuffer);
    std::cout << "Image saved to " << filename << std::endl;
}