#pragma once

#include "Exporter.h"

class PLYExporter : public IExporter {
public:
    bool exportToFile(const std::string& filename, const Mesh& mesh) override;
};