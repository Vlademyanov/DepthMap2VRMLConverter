#pragma once

#include <vector>
#include <string>

class DepthMap {
public:
    DepthMap();
    ~DepthMap();

    bool loadFromFile(const std::string& filename);

    double get(int x, int y) const;
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    int m_width;
    int m_height;
    std::vector<double> m_data;
};