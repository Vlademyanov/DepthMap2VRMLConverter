#include "STLExporter.h"
#include "Mesh.h"
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

bool STLExporter::exportToFile(const std::string& filename, const Mesh& mesh) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot create file: " << filename << std::endl;
        return false;
    }

    const auto& vertices = mesh.getVertices();
    const auto& triangles = mesh.getTriangles();

    file << "solid DepthMapMesh\n";

    for (const auto& tri : triangles) {
        const glm::vec3& v0 = vertices[tri.indices[0]].position;
        const glm::vec3& v1 = vertices[tri.indices[1]].position;
        const glm::vec3& v2 = vertices[tri.indices[2]].position;

        // Calculate face normal
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        file << "  facet normal " << normal.x << " " << normal.y << " " << normal.z << "\n";
        file << "    outer loop\n";
        file << "      vertex " << v0.x << " " << v0.y << " " << v0.z << "\n";
        file << "      vertex " << v1.x << " " << v1.y << " " << v1.z << "\n";
        file << "      vertex " << v2.x << " " << v2.y << " " << v2.z << "\n";
        file << "    endloop\n";
        file << "  endfacet\n";
    }

    file << "endsolid DepthMapMesh\n";

    file.close();
    std::cout << "Exported to " << filename << " (STL ASCII format)" << std::endl;
    return true;
}