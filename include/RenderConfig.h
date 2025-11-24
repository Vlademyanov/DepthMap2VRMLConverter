#pragma once

#include <glm/glm.hpp>

namespace RenderConfig {
    constexpr int DEFAULT_WINDOW_WIDTH = 800;
    constexpr int DEFAULT_WINDOW_HEIGHT = 600;

    constexpr float CAMERA_FOV = 45.0f;
    constexpr float CAMERA_NEAR_PLANE = 0.1f;
    constexpr float CAMERA_FAR_PLANE = 10000.0f;
    constexpr float CAMERA_DISTANCE_MULTIPLIER = 1.5f;
    constexpr float MIN_CAMERA_DISTANCE = 10.0f;

    constexpr float MOUSE_SENSITIVITY = 0.5f;
    constexpr float SCROLL_SENSITIVITY = 10.0f;

    const glm::vec3 BACKGROUND_COLOR(0.1f, 0.1f, 0.15f);
    const glm::vec3 SOLID_COLOR(0.7f, 0.7f, 0.9f);
    const glm::vec3 WIREFRAME_COLOR(0.9f, 0.9f, 1.0f);
    const glm::vec3 WIREFRAME_OVERLAY_COLOR(0.2f, 0.2f, 0.3f);
    const glm::vec3 POINT_COLOR(0.4f, 0.8f, 0.7f);
    const glm::vec3 LIGHT_COLOR(1.0f, 1.0f, 1.0f);

    constexpr float POINT_SIZE = 2.0f;
}