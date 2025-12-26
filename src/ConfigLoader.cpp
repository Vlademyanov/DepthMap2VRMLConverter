#include "../include/ConfigLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

AppConfig ConfigLoader::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Cannot open config file " << filename 
                  << ", using defaults" << std::endl;
        return getDefaultConfig();
    }
    
    try {
        json j;
        file >> j;
        
        AppConfig config;
        
        // Input
        if (j.contains("input")) {
            config.input.depthMapFile = j["input"].value("depthMapFile", "DepthMap_7.dat");
        }
        
        // Output
        if (j.contains("output")) {
            auto& out = j["output"];
            config.output.meshFile = out.value("meshFile", "output.stl");
            config.output.meshFormat = parseMeshFormat(out.value("meshFormat", "stl"));
            config.output.imageFile = out.value("imageFile", "output.bmp");
            config.output.imageWidth = out.value("imageWidth", 800);
            config.output.imageHeight = out.value("imageHeight", 600);
        }
        
        // Lighting
        if (j.contains("lighting")) {
            auto& light = j["lighting"];
            config.lighting.model = parseLightingModel(light.value("model", "phong"));
            
            auto dir = light.value("direction", std::vector<float>{0.0f, 0.0f, 1.0f});
            config.lighting.direction = glm::vec3(dir[0], dir[1], dir[2]);
            
            auto col = light.value("color", std::vector<float>{1.0f, 1.0f, 1.0f});
            config.lighting.color = glm::vec3(col[0], col[1], col[2]);
            
            config.lighting.intensity = light.value("intensity", 1.0f);
            config.lighting.ambientStrength = light.value("ambientStrength", 0.3f);
            config.lighting.diffuseStrength = light.value("diffuseStrength", 0.7f);
            config.lighting.specularStrength = light.value("specularStrength", 0.5f);
            config.lighting.shininess = light.value("shininess", 32.0f);
            config.lighting.roughness = light.value("roughness", 0.5f);
        }
        
        // Camera
        if (j.contains("camera")) {
            auto& cam = j["camera"];
            
            auto pos = cam.value("position", std::vector<float>{0.0f, 0.0f, 100.0f});
            config.camera.position = glm::vec3(pos[0], pos[1], pos[2]);
            
            auto tgt = cam.value("target", std::vector<float>{0.0f, 0.0f, 0.0f});
            config.camera.target = glm::vec3(tgt[0], tgt[1], tgt[2]);
            
            auto up = cam.value("up", std::vector<float>{0.0f, 1.0f, 0.0f});
            config.camera.up = glm::vec3(up[0], up[1], up[2]);
            
            config.camera.fov = cam.value("fov", 45.0f);
            config.camera.nearPlane = cam.value("near", 0.1f);
            config.camera.farPlane = cam.value("far", 10000.0f);
        }
        
        // Rendering
        if (j.contains("rendering")) {
            auto& rend = j["rendering"];
            
            auto bg = rend.value("backgroundColor", std::vector<float>{0.1f, 0.1f, 0.15f});
            config.rendering.backgroundColor = glm::vec3(bg[0], bg[1], bg[2]);
            
            auto obj = rend.value("objectColor", std::vector<float>{0.7f, 0.7f, 0.9f});
            config.rendering.objectColor = glm::vec3(obj[0], obj[1], obj[2]);
        }
        
        return config;
        
    } catch (const json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return getDefaultConfig();
    }
}

AppConfig ConfigLoader::getDefaultConfig() {
    AppConfig config;
    
    config.input.depthMapFile = "DepthMap_7.dat";
    
    config.output.meshFile = "output.stl";
    config.output.meshFormat = MeshFormat::STL;
    config.output.imageFile = "output.bmp";
    config.output.imageWidth = 800;
    config.output.imageHeight = 600;
    
    config.lighting.model = LightingModel::PHONG;
    config.lighting.direction = glm::vec3(0.0f, 0.0f, 1.0f);
    config.lighting.color = glm::vec3(1.0f, 1.0f, 1.0f);
    config.lighting.intensity = 1.0f;
    config.lighting.ambientStrength = 0.3f;
    config.lighting.diffuseStrength = 0.7f;
    config.lighting.specularStrength = 0.5f;
    config.lighting.shininess = 32.0f;
    config.lighting.roughness = 0.5f;
    
    config.camera.position = glm::vec3(0.0f, 0.0f, 100.0f);
    config.camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
    config.camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    config.camera.fov = 45.0f;
    config.camera.nearPlane = 0.1f;
    config.camera.farPlane = 10000.0f;
    
    config.rendering.backgroundColor = glm::vec3(0.1f, 0.1f, 0.15f);
    config.rendering.objectColor = glm::vec3(0.7f, 0.7f, 0.9f);
    
    return config;
}

LightingModel ConfigLoader::parseLightingModel(const std::string& model) {
    if (model == "lambert") return LightingModel::LAMBERT;
    if (model == "phong") return LightingModel::PHONG;
    if (model == "oren-nayar") return LightingModel::OREN_NAYAR;
    return LightingModel::PHONG;
}

MeshFormat ConfigLoader::parseMeshFormat(const std::string& format) {
    if (format == "stl") return MeshFormat::STL;
    if (format == "vrml") return MeshFormat::VRML;
    if (format == "ply") return MeshFormat::PLY;
    return MeshFormat::STL;
}