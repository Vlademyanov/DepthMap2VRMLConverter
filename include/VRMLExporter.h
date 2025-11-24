#pragma once

#include <string>

class Mesh;

class VRMLExporter {
public:
    static bool exportToFile(const std::string& filename, const Mesh& mesh);
};