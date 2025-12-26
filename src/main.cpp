#include "DepthMap.h"
#include "Mesh.h"
#include "Renderer.h"
#include "ImageRenderer.h"
#include "Exporter.h"
#include "Config.h"
#include "ConfigLoader.h"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    std::cout << "=== Depth Map Converter - Lab 4 ===" << std::endl;

    // Load configuration
    std::string configFile = (argc > 1) ? argv[1] : "config.json";
    std::cout << "Loading configuration from: " << configFile << std::endl;

    AppConfig config = ConfigLoader::loadFromFile(configFile);

    std::cout << "\nConfiguration:" << std::endl;
    std::cout << "  Input file: " << config.input.depthMapFile << std::endl;
    std::cout << "  Output mesh: " << config.output.meshFile << std::endl;
    std::cout << "  Output image: " << config.output.imageFile << std::endl;
    std::cout << "  Lighting model: ";
    switch (config.lighting.model) {
        case LightingModel::LAMBERT: std::cout << "Lambert"; break;
        case LightingModel::PHONG: std::cout << "Phong"; break;
        case LightingModel::OREN_NAYAR: std::cout << "Oren-Nayar"; break;
    }
    std::cout << std::endl << std::endl;

    // Check if input file exists
    std::ifstream testFile(config.input.depthMapFile);
    if (!testFile.good()) {
        std::cerr << "Error: Input file not found: " << config.input.depthMapFile << std::endl;
        return 1;
    }
    testFile.close();

    // Load depth map
    std::cout << "Loading depth map..." << std::endl;
    DepthMap depthMap;
    if (!depthMap.loadFromFile(config.input.depthMapFile)) {
        std::cerr << "Error: Failed to load depth map" << std::endl;
        return 1;
    }
    std::cout << "  Dimensions: " << depthMap.getWidth() << "x" << depthMap.getHeight() << std::endl;

    // Build mesh
    std::cout << "\nBuilding mesh..." << std::endl;
    Mesh mesh;
    mesh.buildFromDepthMap(depthMap);
    std::cout << "  Vertices: " << mesh.getVertices().size() << std::endl;
    std::cout << "  Triangles: " << mesh.getTriangles().size() << std::endl;

    // Export mesh
    std::cout << "\nExporting mesh..." << std::endl;
    ExporterFactory::Format format;
    switch (config.output.meshFormat) {
        case MeshFormat::STL:
            format = ExporterFactory::Format::STL;
            break;
        case MeshFormat::VRML:
            format = ExporterFactory::Format::VRML;
            break;
        case MeshFormat::PLY:
            format = ExporterFactory::Format::PLY;
            break;
    }

    auto exporter = ExporterFactory::create(format);
    if (!exporter || !exporter->exportToFile(config.output.meshFile, mesh)) {
        std::cerr << "Error: Failed to export mesh" << std::endl;
        return 1;
    }

    // Render to image
    std::cout << "\nRendering image..." << std::endl;
    ImageRenderer imageRenderer(config.output.imageWidth, config.output.imageHeight);

    // Adjust camera to fit mesh
    BoundingBox bbox = mesh.getBoundingBox();
    CameraConfig camera = config.camera;

    // If camera position is default, calculate optimal position
    if (glm::length(camera.position) < 1.0f) {
        camera.target = bbox.center();
        camera.position = bbox.center() + glm::vec3(0.0f, 0.0f, bbox.maxDimension() * 2.0f);
    }

    imageRenderer.setCamera(camera);
    imageRenderer.setLighting(config.lighting);
    imageRenderer.setBackgroundColor(config.rendering.backgroundColor);
    imageRenderer.setObjectColor(config.rendering.objectColor);

    imageRenderer.renderToImage(mesh, config.output.imageFile);

    // Interactive visualization (optional)
    std::cout << "\nStarting interactive visualization..." << std::endl;
    std::cout << "Press ESC in the window to skip visualization.\n" << std::endl;

    Renderer renderer;
    if (renderer.initialize(800, 600, "Depth Map Visualization - Lab 4")) {
        renderer.loadMesh(mesh);
        renderer.run();
    }

    std::cout << "\nProgram completed successfully!" << std::endl;
    return 0;
}