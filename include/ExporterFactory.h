#pragma once

#include "IExporter.h"
#include "Config.h"

// Factory for creating exporters (Factory pattern)
class ExporterFactory {
public:
    static ExporterPtr create(ExportFormat format);
};