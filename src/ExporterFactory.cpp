#include "ExporterFactory.h"
#include "VRMLExporter.h"
#include "STLExporter.h"
#include "PLYExporter.h"
#include "OBJExporter.h"
#include <iostream>

ExporterPtr ExporterFactory::create(ExportFormat format) {
    switch (format) {
        case ExportFormat::VRML:
            std::cout << "Using VRML exporter" << std::endl;
            return std::make_shared<VRMLExporter>();

        case ExportFormat::STL:
            std::cout << "Using STL exporter" << std::endl;
            return std::make_shared<STLExporter>();

        case ExportFormat::PLY:
            std::cout << "Using PLY exporter" << std::endl;
            return std::make_shared<PLYExporter>();

        case ExportFormat::OBJ:
            std::cout << "Using OBJ exporter" << std::endl;
            return std::make_shared<OBJExporter>();

        default:
            std::cerr << "Unknown export format, defaulting to VRML" << std::endl;
            return std::make_shared<VRMLExporter>();
    }
}