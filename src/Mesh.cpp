#include "Mesh.h"
#include "DepthMap.h"
#include <iostream>
#include <vector>

Mesh::Mesh() {}

Mesh::~Mesh() {}

void Mesh::buildFromDepthMap(const DepthMap& depthMap) {
    m_vertices.clear();
    m_triangles.clear();

    int width = depthMap.getWidth();
    int height = depthMap.getHeight();

    std::vector<int> vertexIndices(height * width, -1);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double depth = depthMap.get(x, y);

            if (depth > 0.0) {
                Vertex vertex;
                vertex.position = glm::vec3(
                        static_cast<float>(x),
                        static_cast<float>(height - 1 -y),
                        static_cast<float>(depth)
                );

                m_vertices.push_back(vertex);
                vertexIndices[y * width + x] = m_vertices.size() - 1;
            }
        }
    }

    for (int y = 0; y < height - 1; y++) {
        for (int x = 0; x < width - 1; x++) {
            int idx00 = vertexIndices[y * width + x];
            int idx10 = vertexIndices[y * width + (x + 1)];
            int idx01 = vertexIndices[(y + 1) * width + x];
            int idx11 = vertexIndices[(y + 1) * width + (x + 1)];

            if (idx00 >= 0 && idx10 >= 0 && idx01 >= 0 && idx11 >= 0) {
                m_triangles.push_back(Triangle(idx00, idx01, idx10));
                m_triangles.push_back(Triangle(idx10, idx01, idx11));
            }
        }
    }

    computeNormals();
}

BoundingBox Mesh::getBoundingBox() const {
    BoundingBox bbox;
    bbox.min = glm::vec3(1e9f);
    bbox.max = glm::vec3(-1e9f);

    for (const auto& v : m_vertices) {
        bbox.min = glm::min(bbox.min, v.position);
        bbox.max = glm::max(bbox.max, v.position);
    }

    return bbox;
}

void Mesh::computeNormals() {
    for (auto& vertex : m_vertices) {
        vertex.normal = glm::vec3(0.0f);
    }

    for (const auto& tri : m_triangles) {
        const glm::vec3& v0 = m_vertices[tri.indices[0]].position;
        const glm::vec3& v1 = m_vertices[tri.indices[1]].position;
        const glm::vec3& v2 = m_vertices[tri.indices[2]].position;

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        m_vertices[tri.indices[0]].normal += normal;
        m_vertices[tri.indices[1]].normal += normal;
        m_vertices[tri.indices[2]].normal += normal;
    }

    for (auto& vertex : m_vertices) {
        vertex.normal = glm::normalize(vertex.normal);
    }
}