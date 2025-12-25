#pragma once

#include <glm/glm.hpp>
#include "Config.h"

// Camera class for view and projection matrices
class Camera {
public:
    Camera();
    Camera(const CameraConfig& config);
    ~Camera() = default;

    void setPosition(const glm::vec3& position);
    void setTarget(const glm::vec3& target);
    void setUp(const glm::vec3& up);
    void setFOV(float fov);
    void setNearFar(float nearPlane, float farPlane);
    void setProjectionType(ProjectionType type);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    glm::mat4 getOrthographicMatrix(float width, float height) const;

    const glm::vec3& getPosition() const { return m_position; }
    const glm::vec3& getTarget() const { return m_target; }
    const glm::vec3& getUp() const { return m_up; }
    glm::vec3 getDirection() const { return glm::normalize(m_target - m_position); }

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    float m_fov;
    float m_nearPlane;
    float m_farPlane;
    ProjectionType m_projectionType;
};