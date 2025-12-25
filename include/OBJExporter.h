#pragma once

#include "IExporter.h"

// OBJ ASCII format exporter
class OBJExporter : public IExporter {
public:
    OBJExporter() = default;
    virtual ~OBJExporter() = default;

    virtual bool exportToFile(const std::string& filename, const Mesh& mesh) override;
    virtual std::string getFormatName() const override { return "Wavefront OBJ"; }
    virtual std::string getFileExtension() const override { return "obj"; }
};