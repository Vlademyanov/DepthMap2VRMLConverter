#pragma once

#include "IExporter.h"

// VRML ASCII format exporter
class VRMLExporter : public IExporter {
public:
    VRMLExporter() = default;
    virtual ~VRMLExporter() = default;

    virtual bool exportToFile(const std::string& filename, const Mesh& mesh) override;
    virtual std::string getFormatName() const override { return "VRML 1.0"; }
    virtual std::string getFileExtension() const override { return "vrml"; }
};