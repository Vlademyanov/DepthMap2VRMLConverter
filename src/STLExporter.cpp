#include "STLExporter.h"
#include "Mesh.h"
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

bool STLExporter::exportToFile(const std::string& filename, const Mesh& mesh) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot create STL file: " << filename << std::endl;
        return false;
    }
    
    const auto& vertices = mesh.getVertices();
    const auto& triangles = mesh.getTriangles();
    
    // STL ASCII format
    file << "solid DepthMapMesh\n";
    
    for (const auto& tri : triangles) {
        const Vertex& v0 = vertices[tri.indices[0]];
        const Vertex& v1 = vertices[tri.indices[1]];
        const Vertex& v2 = vertices[tri.indices[2]];
        
        // Calculate face normal
        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        
        file << "  facet normal " << normal.x << " " << normal.y << " " << normal.z << "\n";
        file << "    outer loop\n";
        file << "      vertex " << v0.position.x << " " << v0.position.y << " " << v0.position.z << "\n";
        file << "      vertex " << v1.position.x << " " << v1.position.y << " " << v1.position.z << "\n";
        file << "      vertex " << v2.position.x << " " << v2.position.y << " " << v2.position.z << "\n";
        file << "    endloop\n";
        file << "  endfacet\n";
    }
    
    file << "endsolid DepthMapMesh\n";
    
    file.close();
    std::cout << "Mesh exported to " << filename << " (STL format)" << std::endl;
    return true;
}