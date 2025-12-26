#include "BMPWriter.h"
#include <fstream>
#include <iostream>
#include <algorithm>

bool BMPWriter::write(const std::string& filename, 
                     int width, int height, 
                     const std::vector<glm::vec3>& pixels) {
    if (width <= 0 || height <= 0 || pixels.size() != width * height) {
        std::cerr << "Invalid BMP dimensions or pixel data" << std::endl;
        return false;
    }
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Cannot create BMP file: " << filename << std::endl;
        return false;
    }
    
    // BMP padding (rows must be multiple of 4 bytes)
    int rowPadding = (4 - (width * 3) % 4) % 4;
    int rowSize = width * 3 + rowPadding;
    int imageSize = rowSize * height;
    
    BMPFileHeader fileHeader;
    fileHeader.fileSize = 54 + imageSize;
    
    BMPInfoHeader infoHeader;
    infoHeader.width = width;
    infoHeader.height = height;
    infoHeader.sizeImage = imageSize;
    
    // Write headers
    file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));
    
    // Write pixel data (BMP stores bottom-to-top, BGR format)
    std::vector<uint8_t> padding(rowPadding, 0);
    
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            const glm::vec3& pixel = pixels[y * width + x];
            
            // Convert float [0,1] to uint8 [0,255] and write as BGR
            uint8_t b = static_cast<uint8_t>(std::clamp(pixel.b * 255.0f, 0.0f, 255.0f));
            uint8_t g = static_cast<uint8_t>(std::clamp(pixel.g * 255.0f, 0.0f, 255.0f));
            uint8_t r = static_cast<uint8_t>(std::clamp(pixel.r * 255.0f, 0.0f, 255.0f));
            
            file.write(reinterpret_cast<const char*>(&b), 1);
            file.write(reinterpret_cast<const char*>(&g), 1);
            file.write(reinterpret_cast<const char*>(&r), 1);
        }
        
        if (rowPadding > 0) {
            file.write(reinterpret_cast<const char*>(padding.data()), rowPadding);
        }
    }
    
    file.close();
    return true;
}