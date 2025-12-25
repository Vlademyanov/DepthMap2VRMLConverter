#pragma once

#include "IExporter.h"

// STL ASCII format exporter
class STLExporter : public IExporter {
public:
    STLExporter() = default;
    virtual ~STLExporter() = default;

    virtual bool exportToFile(const std::string& filename, const Mesh& mesh) override;
    virtual std::string getFormatName() const override { return "STL ASCII"; }
    virtual std::string getFileExtension() const override { return "stl"; }
};