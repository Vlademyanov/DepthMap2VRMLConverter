#include "DepthMap.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Config.h"
#include "ExporterFactory.h"
#include "LightingFactory.h"
#include "ImageRenderer.h"
#include <iostream>
#include <fstream>

void printUsage(const char* programName) {
    std::cout << "\nUsage: " << programName << " [config.json]\n";
    std::cout << "\nIf no config file is provided, default configuration will be used.\n";
    std::cout << "\nExample config.json structure:\n";
    std::cout << R"({
  "inputFile": "DepthMap_7.dat",
  "outputFile": "output.vrml",
  "exportFormat": "vrml",
  "lightingModel": "phong",
  "enableVisualization": true,
  "camera": {
    "position": [0, 0, 100],
    "target": [0, 0, 0],
    "up": [0, 1, 0],
    "fov": 45,
    "nearPlane": 0.1,
    "farPlane": 10000,
    "projectionType": "perspective"
  },
  "light": {
    "direction": [1, 1, 1],
    "color": [1, 1, 1],
    "intensity": 1.0
  },
  "renderImage": {
    "enabled": true,
    "width": 800,
    "height": 600,
    "outputFile": "render.bmp"
  },
  "material": {
    "roughness": 0.5,
    "diffuseColor": [0.7, 0.7, 0.9],
    "specularColor": [1.0, 1.0, 1.0],
    "shininess": 32.0
  }
})" << std::endl;
    std::cout << "\nSupported export formats: vrml, stl, ply, obj\n";
    std::cout << "Supported lighting models: lambert, phong, oren-nayar\n\n";
}

int main(int argc, char** argv) {
    std::cout << "=================================================\n";
    std::cout << "     Depth Map Converter - 3D Surface Renderer\n";
    std::cout << "=================================================\n\n";

    // Load configuration
    Config config;
    std::string configFile = (argc > 1) ? argv[1] : "config.json";

    if (argc > 1 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        printUsage(argv[0]);
        return 0;
    }

    config.loadFromFile(configFile);

    std::cout << "\n--- Configuration ---" << std::endl;
    std::cout << "Input file: " << config.getInputFile() << std::endl;
    std::cout << "Output file: " << config.getOutputFile() << std::endl;
    std::cout << "Visualization: " << (config.isVisualizationEnabled() ? "Enabled" : "Disabled") << std::endl;

    // Check if input file exists
    std::ifstream testFile(config.getInputFile());
    if (!testFile.good()) {
        std::cerr << "Error: Input file not found: " << config.getInputFile() << std::endl;
        std::cerr << "Please provide a valid depth map file.\n" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    testFile.close();

    // Load depth map
    std::cout << "\n--- Loading Depth Map ---" << std::endl;
    DepthMap depthMap;
    if (!depthMap.loadFromFile(config.getInputFile())) {
        std::cerr << "Failed to load depth map" << std::endl;
        return 1;
    }
    std::cout << "Depth map loaded successfully" << std::endl;
    std::cout << "  Dimensions: " << depthMap.getWidth() << " x " << depthMap.getHeight() << std::endl;

    // Build mesh
    std::cout << "\n--- Building Mesh ---" << std::endl;
    Mesh mesh;
    mesh.buildFromDepthMap(depthMap);
    std::cout << "Mesh built successfully" << std::endl;
    std::cout << "  Vertices: " << mesh.getVertices().size() << std::endl;
    std::cout << "  Triangles: " << mesh.getTriangles().size() << std::endl;

    // Export mesh
    std::cout << "\n--- Exporting Mesh ---" << std::endl;
    ExporterPtr exporter = ExporterFactory::create(config.getExportFormat());
    if (!exporter->exportToFile(config.getOutputFile(), mesh)) {
        std::cerr << "Failed to export mesh" << std::endl;
        return 1;
    }

    // Render to image if enabled
    if (config.getRenderImageConfig().enabled) {
        std::cout << "\n--- Rendering Image ---" << std::endl;
        LightingModelPtr lightingModel = LightingFactory::create(
                config.getLightingModel(), config
        );

        ImageRenderer imageRenderer;
        if (!imageRenderer.renderToFile(mesh, config, lightingModel)) {
            std::cerr << "Failed to render image" << std::endl;
            return 1;
        }
    }

    // Interactive visualization
    if (config.isVisualizationEnabled()) {
        std::cout << "\n--- Starting Interactive Visualization ---" << std::endl;
        std::cout << "Initializing OpenGL renderer..." << std::endl;

        Renderer renderer;
        if (!renderer.initialize(800, 600, "Depth Map 3D Visualization")) {
            std::cerr << "Failed to initialize renderer" << std::endl;
            return 1;
        }

        renderer.loadMesh(mesh);
        renderer.run();
    }

    std::cout << "\n=================================================\n";
    std::cout << "               Processing Complete\n";
    std::cout << "=================================================\n" << std::endl;

    return 0;
}