#pragma once

#include <string>
#include <memory>

class Mesh;

class IExporter {
public:
    virtual ~IExporter() = default;
    virtual bool exportToFile(const std::string& filename, const Mesh& mesh) = 0;
};

class ExporterFactory {
public:
    enum class Format {
        STL,
        VRML,
        PLY
    };
    
    static std::unique_ptr<IExporter> create(Format format);
};