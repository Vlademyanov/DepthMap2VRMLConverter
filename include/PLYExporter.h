#pragma once

#include "IExporter.h"

// PLY ASCII format exporter
class PLYExporter : public IExporter {
public:
    PLYExporter() = default;
    virtual ~PLYExporter() = default;

    virtual bool exportToFile(const std::string& filename, const Mesh& mesh) override;
    virtual std::string getFormatName() const override { return "PLY ASCII"; }
    virtual std::string getFileExtension() const override { return "ply"; }
};