#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

// BMP file writer
class BMPWriter {
public:
    static bool write(
            const std::string& filename,
            int width,
            int height,
            const std::vector<glm::vec3>& pixels
    );

private:
#pragma pack(push, 1)
    struct BMPFileHeader {
        uint16_t fileType{0x4D42};      // BM
        uint32_t fileSize{0};
        uint16_t reserved1{0};
        uint16_t reserved2{0};
        uint32_t offsetData{54};
    };

    struct BMPInfoHeader {
        uint32_t size{40};
        int32_t width{0};
        int32_t height{0};
        uint16_t planes{1};
        uint16_t bitCount{24};
        uint32_t compression{0};
        uint32_t sizeImage{0};
        int32_t xPixelsPerMeter{0};
        int32_t yPixelsPerMeter{0};
        uint32_t colorsUsed{0};
        uint32_t colorsImportant{0};
    };
#pragma pack(pop)
};