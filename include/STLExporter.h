#pragma once

#include "Exporter.h"

class STLExporter : public IExporter {
public:
    bool exportToFile(const std::string& filename, const Mesh& mesh) override;
};