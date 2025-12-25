#include "LightingFactory.h"
#include "LambertLighting.h"
#include "PhongLighting.h"
#include "OrenNayarLighting.h"
#include <iostream>

LightingModelPtr LightingFactory::create(LightingModel type, const Config& config) {
    switch (type) {
        case LightingModel::LAMBERT:
            std::cout << "Using Lambert lighting model" << std::endl;
            return std::make_shared<LambertLighting>();

        case LightingModel::PHONG: {
            std::cout << "Using Phong lighting model" << std::endl;
            auto phong = std::make_shared<PhongLighting>();
            phong->setShininess(config.getShininess());
            phong->setSpecularColor(config.getSpecularColor());
            return phong;
        }

        case LightingModel::OREN_NAYAR: {
            std::cout << "Using Oren-Nayar lighting model" << std::endl;
            auto orenNayar = std::make_shared<OrenNayarLighting>();
            orenNayar->setRoughness(config.getRoughness());
            return orenNayar;
        }

        default:
            std::cerr << "Unknown lighting model, defaulting to Phong" << std::endl;
            return std::make_shared<PhongLighting>();
    }
}