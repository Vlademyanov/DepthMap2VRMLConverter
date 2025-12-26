#pragma once

#include "Config.h"
#include <string>

class ConfigLoader {
public:
    static AppConfig loadFromFile(const std::string& filename);
    static AppConfig getDefaultConfig();
    
private:
    static LightingModel parseLightingModel(const std::string& model);
    static MeshFormat parseMeshFormat(const std::string& format);
};