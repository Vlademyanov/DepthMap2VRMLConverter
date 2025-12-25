#include "Config.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

Config::Config() {
    setDefaults();
}

Config::~Config() {}

void Config::setDefaults() {
    m_inputFile = "DepthMap_7.dat";
    m_outputFile = "output.vrml";
    m_exportFormat = ExportFormat::VRML;
    m_lightingModel = LightingModel::PHONG;
    m_enableVisualization = true;

    m_cameraConfig.position = glm::vec3(0.0f, 0.0f, 100.0f);
    m_cameraConfig.target = glm::vec3(0.0f, 0.0f, 0.0f);
    m_cameraConfig.up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_cameraConfig.fov = 45.0f;
    m_cameraConfig.nearPlane = 0.1f;
    m_cameraConfig.farPlane = 10000.0f;
    m_cameraConfig.projectionType = ProjectionType::PERSPECTIVE;

    m_lightConfig.direction = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
    m_lightConfig.color = glm::vec3(1.0f, 1.0f, 1.0f);
    m_lightConfig.intensity = 1.0f;

    m_renderImageConfig.width = 800;
    m_renderImageConfig.height = 600;
    m_renderImageConfig.outputFile = "output.bmp";
    m_renderImageConfig.enabled = false;

    m_roughness = 0.5f;
    m_diffuseColor = glm::vec3(0.7f, 0.7f, 0.9f);
    m_specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
    m_shininess = 32.0f;
}

bool Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Cannot open config file: " << filename << std::endl;
        std::cerr << "Using default configuration" << std::endl;
        return false;
    }

    try {
        json j;
        file >> j;

        if (j.contains("inputFile")) {
            m_inputFile = j["inputFile"].get<std::string>();
        }

        if (j.contains("outputFile")) {
            m_outputFile = j["outputFile"].get<std::string>();
        }

        if (j.contains("exportFormat")) {
            m_exportFormat = parseExportFormat(j["exportFormat"].get<std::string>());
        }

        if (j.contains("lightingModel")) {
            m_lightingModel = parseLightingModel(j["lightingModel"].get<std::string>());
        }

        if (j.contains("enableVisualization")) {
            m_enableVisualization = j["enableVisualization"].get<bool>();
        }

        // Camera configuration
        if (j.contains("camera")) {
            auto& cam = j["camera"];
            if (cam.contains("position") && cam["position"].is_array() && cam["position"].size() == 3) {
                m_cameraConfig.position = glm::vec3(
                        cam["position"][0].get<float>(),
                        cam["position"][1].get<float>(),
                        cam["position"][2].get<float>()
                );
            }
            if (cam.contains("target") && cam["target"].is_array() && cam["target"].size() == 3) {
                m_cameraConfig.target = glm::vec3(
                        cam["target"][0].get<float>(),
                        cam["target"][1].get<float>(),
                        cam["target"][2].get<float>()
                );
            }
            if (cam.contains("up") && cam["up"].is_array() && cam["up"].size() == 3) {
                m_cameraConfig.up = glm::vec3(
                        cam["up"][0].get<float>(),
                        cam["up"][1].get<float>(),
                        cam["up"][2].get<float>()
                );
            }
            if (cam.contains("fov")) {
                m_cameraConfig.fov = cam["fov"].get<float>();
            }
            if (cam.contains("nearPlane")) {
                m_cameraConfig.nearPlane = cam["nearPlane"].get<float>();
            }
            if (cam.contains("farPlane")) {
                m_cameraConfig.farPlane = cam["farPlane"].get<float>();
            }
            if (cam.contains("projectionType")) {
                m_cameraConfig.projectionType = parseProjectionType(cam["projectionType"].get<std::string>());
            }
        }

        // Light configuration
        if (j.contains("light")) {
            auto& light = j["light"];
            if (light.contains("direction") && light["direction"].is_array() && light["direction"].size() == 3) {
                glm::vec3 dir(
                        light["direction"][0].get<float>(),
                        light["direction"][1].get<float>(),
                        light["direction"][2].get<float>()
                );
                m_lightConfig.direction = glm::normalize(dir);
            }
            if (light.contains("color") && light["color"].is_array() && light["color"].size() == 3) {
                m_lightConfig.color = glm::vec3(
                        light["color"][0].get<float>(),
                        light["color"][1].get<float>(),
                        light["color"][2].get<float>()
                );
            }
            if (light.contains("intensity")) {
                m_lightConfig.intensity = light["intensity"].get<float>();
            }
        }

        // Image render configuration
        if (j.contains("renderImage")) {
            auto& img = j["renderImage"];
            if (img.contains("enabled")) {
                m_renderImageConfig.enabled = img["enabled"].get<bool>();
            }
            if (img.contains("width")) {
                m_renderImageConfig.width = img["width"].get<int>();
            }
            if (img.contains("height")) {
                m_renderImageConfig.height = img["height"].get<int>();
            }
            if (img.contains("outputFile")) {
                m_renderImageConfig.outputFile = img["outputFile"].get<std::string>();
            }
        }

        // Material properties
        if (j.contains("material")) {
            auto& mat = j["material"];
            if (mat.contains("roughness")) {
                m_roughness = mat["roughness"].get<float>();
            }
            if (mat.contains("diffuseColor") && mat["diffuseColor"].is_array() && mat["diffuseColor"].size() == 3) {
                m_diffuseColor = glm::vec3(
                        mat["diffuseColor"][0].get<float>(),
                        mat["diffuseColor"][1].get<float>(),
                        mat["diffuseColor"][2].get<float>()
                );
            }
            if (mat.contains("specularColor") && mat["specularColor"].is_array() && mat["specularColor"].size() == 3) {
                m_specularColor = glm::vec3(
                        mat["specularColor"][0].get<float>(),
                        mat["specularColor"][1].get<float>(),
                        mat["specularColor"][2].get<float>()
                );
            }
            if (mat.contains("shininess")) {
                m_shininess = mat["shininess"].get<float>();
            }
        }

        std::cout << "Configuration loaded from " << filename << std::endl;
        return true;

    } catch (const json::exception& e) {
        std::cerr << "Error parsing JSON config: " << e.what() << std::endl;
        std::cerr << "Using default configuration" << std::endl;
        return false;
    }
}

LightingModel Config::parseLightingModel(const std::string& model) {
    if (model == "lambert" || model == "Lambert" || model == "LAMBERT") {
        return LightingModel::LAMBERT;
    } else if (model == "phong" || model == "Phong" || model == "PHONG") {
        return LightingModel::PHONG;
    } else if (model == "oren-nayar" || model == "Oren-Nayar" || model == "OREN_NAYAR") {
        return LightingModel::OREN_NAYAR;
    }
    std::cerr << "Unknown lighting model: " << model << ", using Phong" << std::endl;
    return LightingModel::PHONG;
}

ExportFormat Config::parseExportFormat(const std::string& format) {
    if (format == "vrml" || format == "VRML" || format == ".vrml") {
        return ExportFormat::VRML;
    } else if (format == "stl" || format == "STL" || format == ".stl") {
        return ExportFormat::STL;
    } else if (format == "ply" || format == "PLY" || format == ".ply") {
        return ExportFormat::PLY;
    } else if (format == "obj" || format == "OBJ" || format == ".obj") {
        return ExportFormat::OBJ;
    }
    std::cerr << "Unknown export format: " << format << ", using VRML" << std::endl;
    return ExportFormat::VRML;
}

ProjectionType Config::parseProjectionType(const std::string& type) {
    if (type == "orthographic" || type == "Orthographic" || type == "ORTHOGRAPHIC") {
        return ProjectionType::ORTHOGRAPHIC;
    }
    return ProjectionType::PERSPECTIVE;
}