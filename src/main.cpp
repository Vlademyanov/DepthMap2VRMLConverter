#include "DepthMap.h"
#include "Mesh.h"
#include "VRMLExporter.h"
#include "Renderer.h"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    std::string inputFile = (argc > 1) ? argv[1] : "DepthMap_7.dat";
    std::string outputFile = (argc > 2) ? argv[2] : "output.vrml";

    std::ifstream testFile(inputFile);
    if (!testFile.good()) {
        return 1;
    }
    testFile.close();

    DepthMap depthMap;
    if (!depthMap.loadFromFile(inputFile)) {
        std::cerr << "Depth map load error" << std::endl;
        return 1;
    }

    Mesh mesh;
    mesh.buildFromDepthMap(depthMap);

    if (!VRMLExporter::exportToFile(outputFile, mesh)) {
        std::cerr << "VRML export error" << std::endl;
        return 1;
    }

    Renderer renderer;
    if (!renderer.initialize(800, 600, "Визуализация карты глубины - ЛР №3")) {
        std::cerr << "Render init error" << std::endl;
        return 1;
    }

    renderer.loadMesh(mesh);
    renderer.run();
    return 0;
}