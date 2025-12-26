#include "PLYExporter.h"
#include "Mesh.h"
#include <fstream>
#include <iostream>

bool PLYExporter::exportToFile(const std::string& filename, const Mesh& mesh) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot create PLY file: " << filename << std::endl;
        return false;
    }
    
    const auto& vertices = mesh.getVertices();
    const auto& triangles = mesh.getTriangles();
    
    // PLY ASCII format header
    file << "ply\n";
    file << "format ascii 1.0\n";
    file << "comment Created by DepthMapConverter\n";
    file << "element vertex " << vertices.size() << "\n";
    file << "property float x\n";
    file << "property float y\n";
    file << "property float z\n";
    file << "property float nx\n";
    file << "property float ny\n";
    file << "property float nz\n";
    file << "element face " << triangles.size() << "\n";
    file << "property list uchar int vertex_indices\n";
    file << "end_header\n";
    
    // Write vertices
    for (const auto& v : vertices) {
        file << v.position.x << " " << v.position.y << " " << v.position.z << " "
             << v.normal.x << " " << v.normal.y << " " << v.normal.z << "\n";
    }
    
    // Write faces
    for (const auto& tri : triangles) {
        file << "3 " << tri.indices[0] << " " << tri.indices[1] << " " << tri.indices[2] << "\n";
    }
    
    file.close();
    std::cout << "Mesh exported to " << filename << " (PLY format)" << std::endl;
    return true;
}