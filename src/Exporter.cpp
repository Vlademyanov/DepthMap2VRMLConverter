#include "Exporter.h"
#include "STLExporter.h"
#include "VRMLExporter.h"
#include "PLYExporter.h"

std::unique_ptr<IExporter> ExporterFactory::create(Format format) {
    switch (format) {
        case Format::STL:
            return std::make_unique<STLExporter>();
        case Format::VRML:
            return std::make_unique<VRMLExporter>();
        case Format::PLY:
            return std::make_unique<PLYExporter>();
    }
    return nullptr;
}