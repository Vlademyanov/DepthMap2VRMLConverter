#include "BMPWriter.h"
#include <fstream>
#include <iostream>
#include <algorithm>

bool BMPWriter::write(const std::string& filename, int width, int height,
                      const std::vector<glm::vec3>& pixels) {
    if (pixels.size() != static_cast<size_t>(width * height)) {
        std::cerr << "Invalid pixel data size" << std::endl;
        return false;
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Cannot create BMP file: " << filename << std::endl;
        return false;
    }

    // Calculate row size (must be multiple of 4)
    int rowSize = ((width * 3 + 3) / 4) * 4;
    int padding = rowSize - width * 3;

    BMPFileHeader fileHeader;
    fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + rowSize * height;

    BMPInfoHeader infoHeader;
    infoHeader.width = width;
    infoHeader.height = height;
    infoHeader.sizeImage = rowSize * height;

    // Write headers
    file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(BMPFileHeader));
    file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(BMPInfoHeader));

    // Write pixel data (BMP stores bottom-to-top)
    std::vector<unsigned char> row(rowSize, 0);
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            const glm::vec3& pixel = pixels[y * width + x];

            // Convert float [0,1] to byte [0,255] and clamp
            unsigned char b = static_cast<unsigned char>(
                    std::clamp(pixel.b * 255.0f, 0.0f, 255.0f));
            unsigned char g = static_cast<unsigned char>(
                    std::clamp(pixel.g * 255.0f, 0.0f, 255.0f));
            unsigned char r = static_cast<unsigned char>(
                    std::clamp(pixel.r * 255.0f, 0.0f, 255.0f));

            // BMP stores in BGR format
            row[x * 3 + 0] = b;
            row[x * 3 + 1] = g;
            row[x * 3 + 2] = r;
        }
        file.write(reinterpret_cast<const char*>(row.data()), rowSize);
    }

    file.close();
    std::cout << "Image saved to " << filename << " (" << width << "x" << height << ")" << std::endl;
    return true;
}