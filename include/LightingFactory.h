#pragma once

#include "ILightingModel.h"
#include "Config.h"

// Factory for creating lighting models (Factory pattern)
class LightingFactory {
public:
    static LightingModelPtr create(
            LightingModel type,
            const Config& config
    );
};