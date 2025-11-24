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

class Mesh {
public:
    Mesh();
    ~Mesh();

    void buildFromDepthMap(const DepthMap& depthMap);

    void computeNormals();

    const std::vector<Vertex>& getVertices() const { return m_vertices; }
    const std::vector<Triangle>& getTriangles() const { return m_triangles; }

private:
    std::vector<Vertex> m_vertices;
    std::vector<Triangle> m_triangles;
};