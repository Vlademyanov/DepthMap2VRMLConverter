#include "VRMLExporter.h"
#include "Mesh.h"
#include <fstream>
#include <iostream>

bool VRMLExporter::exportToFile(const std::string& filename, const Mesh& mesh) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Can't create " << filename << std::endl;
        return false;
    }

    const auto& vertices = mesh.getVertices();
    const auto& triangles = mesh.getTriangles();

    file << "#VRML V1.0 ascii\n";

    file << "Separator {\n";

    file << "    Material {\n";
    file << "        diffuseColor 0.8 0.8 0.9\n";
    file << "        specularColor 0.5 0.5 0.5\n";
    file << "        shininess 0.3\n";
    file << "    }\n";

    file << "    ShapeHints {\n";
    file << "        vertexOrdering COUNTERCLOCKWISE\n";
    file << "        shapeType SOLID\n";
    file << "        faceType CONVEX\n";
    file << "    }\n";

    file << "    Coordinate3 {\n";
    file << "        point [\n";
    for (size_t i = 0; i < vertices.size(); i++) {
        const auto& v = vertices[i].position;
        file << "            " << v.x << " " << v.y << " " << v.z;
        if (i < vertices.size() - 1) file << ",";
        file << "\n";
    }
    file << "        ]\n";
    file << "    }\n";

    file << "    IndexedFaceSet {\n";
    file << "        coordIndex [\n";
    for (size_t i = 0; i < triangles.size(); i++) {
        const auto& t = triangles[i];
        file << "            " << t.indices[0] << ", " << t.indices[1] << ", "
             << t.indices[2] << ", -1";
        if (i < triangles.size() - 1) file << ",";
        file << "\n";
    }
    file << "        ]\n";
    file << "    }\n";
    file << "}\n";

    file.close();
    std::cout << "Exported to " << filename << " (VRML format)" << std::endl;
    return true;
}