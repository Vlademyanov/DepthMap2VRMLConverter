#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
        : m_position(0.0f, 0.0f, 100.0f)
        , m_target(0.0f, 0.0f, 0.0f)
        , m_up(0.0f, 1.0f, 0.0f)
        , m_fov(45.0f)
        , m_nearPlane(0.1f)
        , m_farPlane(10000.0f)
        , m_projectionType(ProjectionType::PERSPECTIVE) {
}

Camera::Camera(const CameraConfig& config)
        : m_position(config.position)
        , m_target(config.target)
        , m_up(config.up)
        , m_fov(config.fov)
        , m_nearPlane(config.nearPlane)
        , m_farPlane(config.farPlane)
        , m_projectionType(config.projectionType) {
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
}

void Camera::setTarget(const glm::vec3& target) {
    m_target = target;
}

void Camera::setUp(const glm::vec3& up) {
    m_up = up;
}

void Camera::setFOV(float fov) {
    m_fov = fov;
}

void Camera::setNearFar(float nearPlane, float farPlane) {
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
}

void Camera::setProjectionType(ProjectionType type) {
    m_projectionType = type;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    if (m_projectionType == ProjectionType::ORTHOGRAPHIC) {
        float height = 100.0f; // Default orthographic height
        float width = height * aspectRatio;
        return glm::ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f,
                          m_nearPlane, m_farPlane);
    }
    return glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
}

glm::mat4 Camera::getOrthographicMatrix(float width, float height) const {
    return glm::ortho(0.0f, width, 0.0f, height, m_nearPlane, m_farPlane);
}