#pragma once

#include <string>
#include <memory>

class Mesh;

// Interface for 3D format exporters (Strategy pattern)
class IExporter {
public:
    virtual ~IExporter() = default;

    // Export mesh to file
    virtual bool exportToFile(const std::string& filename, const Mesh& mesh) = 0;

    // Get exporter name/format
    virtual std::string getFormatName() const = 0;

    // Get file extension (without dot)
    virtual std::string getFileExtension() const = 0;
};

using ExporterPtr = std::shared_ptr<IExporter>;