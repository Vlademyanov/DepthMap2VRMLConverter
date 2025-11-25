#pragma once

#include <vector>
#include <glm/glm.hpp>

class DepthMap;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;

    Vertex() : position(0.0f), normal(0.0f, 0.0f, 1.0f) {}
    Vertex(const glm::vec3& pos) : position(pos), normal(0.0f, 0.0f, 1.0f) {}
};

struct Triangle {
    unsigned int indices[3];

    Triangle(unsigned int i0, unsigned int i1, unsigned int i2) {
        indices[0] = i0;
        indices[1] = i1;
        indices[2] = i2;
    }
};

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center() const { return (min + max) * 0.5f; }
    glm::vec3 size() const { return max - min; }
    float maxDimension() const {
        glm::vec3 s = size();
        return glm::max(glm::max(s.x, s.y), s.z);
    }
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    void buildFromDepthMap(const DepthMap& depthMap);
    BoundingBox getBoundingBox() const;
    void computeNormals();

    const std::vector<Vertex>& getVertices() const { return m_vertices; }
    const std::vector<Triangle>& getTriangles() const { return m_triangles; }

private:
    std::vector<Vertex> m_vertices;
    std::vector<Triangle> m_triangles;
};