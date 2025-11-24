#include "DepthMap.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>

DepthMap::DepthMap() : m_width(0), m_height(0) {}

DepthMap::~DepthMap() {}

bool DepthMap::loadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Can't open " << filename << std::endl;
        return false;
    }

    double heightDouble, widthDouble;
    file.read(reinterpret_cast<char*>(&heightDouble), sizeof(double));
    file.read(reinterpret_cast<char*>(&widthDouble), sizeof(double));

    if (!file) {
        std::cerr << "Dimensions read error" << std::endl;
        return false;
    }

    m_height = static_cast<int>(heightDouble);
    m_width = static_cast<int>(widthDouble);

    if (m_height <= 0 || m_width <= 0) {
        std::cerr << "Invalid dimensions error" << std::endl;
        std::cerr << "  Width: " << m_width << ", Height: " << m_height << std::endl;
        return false;
    }

    int totalPixels = m_height * m_width;
    m_data.resize(totalPixels);

    file.read(reinterpret_cast<char*>(m_data.data()), totalPixels * sizeof(double));

    if (!file) {
        std::cerr << "Depth map read error" << std::endl;
        return false;
    }

    file.close();
    return true;
}

double DepthMap::get(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return 0.0;
    }
    return m_data[y * m_width + x];
}