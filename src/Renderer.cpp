#include "Renderer.h"
#include "Mesh.h"
#include "ShaderLoader.h"
#include "RenderConfig.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

Renderer::Renderer()
        : m_window(nullptr)
        , m_vao(0)
        , m_vbo(0)
        , m_ebo(0)
        , m_shaderProgram(0)
        , m_indexCount(0)
        , m_vertexCount(0)
        , m_renderMode(RenderMode::SOLID)
        , m_cameraPos(0.0f, 0.0f, 100.0f)
        , m_cameraTarget(0.0f, 0.0f, 0.0f)
        , m_rotationX(0.0f)
        , m_rotationY(0.0f)
        , m_lastMouseX(0.0)
        , m_lastMouseY(0.0)
        , m_mousePressed(false) {}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize(int width, int height, const std::string& title) {
    if (!glfwInit()) {
        std::cerr << "GLFW init error" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "GLFW window create error" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    glfwSetKeyCallback(m_window, keyCallback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK && glewInit() != 4) {
        std::cerr << "GLEW init error: " << std::endl;
        glfwDestroyWindow(m_window);
        glfwTerminate();
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(RenderConfig::BACKGROUND_COLOR.r,
                 RenderConfig::BACKGROUND_COLOR.g,
                 RenderConfig::BACKGROUND_COLOR.b, 1.0f);

    if (!createShaders()) {
        return false;
    }

    return true;
}

bool Renderer::createShaders() {
    m_shaderProgram = ShaderLoader::loadProgram("shaders/vertex.glsl", "shaders/fragment.glsl");
    if (m_shaderProgram == 0) {
        std::cerr << "Shader program load error" << std::endl;
        return false;
    }

    return true;
}

void Renderer::loadMesh(const Mesh& mesh) {
    const auto& vertices = mesh.getVertices();
    const auto& triangles = mesh.getTriangles();

    m_indexCount = triangles.size() * 3;
    m_vertexCount = vertices.size();

    std::vector<float> vertexData;
    for (const auto& v : vertices) {
        vertexData.push_back(v.position.x);
        vertexData.push_back(v.position.y);
        vertexData.push_back(v.position.z);
        vertexData.push_back(v.normal.x);
        vertexData.push_back(v.normal.y);
        vertexData.push_back(v.normal.z);
    }

    std::vector<unsigned int> indexData;
    for (const auto& t : triangles) {
        indexData.push_back(t.indices[0]);
        indexData.push_back(t.indices[1]);
        indexData.push_back(t.indices[2]);
    }

    BoundingBox bbox = mesh.getBoundingBox();
    m_cameraTarget = bbox.center();
    m_cameraPos = bbox.center() + glm::vec3(0.0f, 0.0f,bbox.maxDimension() * RenderConfig::CAMERA_DISTANCE_MULTIPLIER);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float),
                 vertexData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int),
                 indexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shaderProgram);

    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    float aspect = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_cameraTarget);
    model = glm::rotate(model, glm::radians(m_rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, -m_cameraTarget);

    glm::mat4 view = glm::lookAt(m_cameraPos, m_cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 projection = glm::perspective(
            glm::radians(RenderConfig::CAMERA_FOV),
            aspect,
            RenderConfig::CAMERA_NEAR_PLANE,
            RenderConfig::CAMERA_FAR_PLANE
    );

    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "model"), 1, GL_FALSE,
                       glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));

    glUniform3fv(glGetUniformLocation(m_shaderProgram, "lightPos"), 1,
                 glm::value_ptr(m_cameraPos));
    glUniform3fv(glGetUniformLocation(m_shaderProgram, "viewPos"), 1,
                 glm::value_ptr(m_cameraPos));
    glUniform3fv(glGetUniformLocation(m_shaderProgram, "lightColor"), 1,
                 glm::value_ptr(RenderConfig::LIGHT_COLOR));

    glBindVertexArray(m_vao);

    switch (m_renderMode) {
        case RenderMode::SOLID:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glUniform3fv(glGetUniformLocation(m_shaderProgram, "objectColor"), 1,
                         glm::value_ptr(RenderConfig::SOLID_COLOR));
            glUniform1i(glGetUniformLocation(m_shaderProgram, "usePointColor"), 0);
            glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
            break;

        case RenderMode::WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUniform3fv(glGetUniformLocation(m_shaderProgram, "objectColor"), 1,
                         glm::value_ptr(RenderConfig::WIREFRAME_COLOR));
            glUniform1i(glGetUniformLocation(m_shaderProgram, "usePointColor"), 0);
            glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
            break;

        case RenderMode::SOLID_WITH_WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glUniform3fv(glGetUniformLocation(m_shaderProgram, "objectColor"), 1,
                         glm::value_ptr(RenderConfig::SOLID_COLOR));
            glUniform1i(glGetUniformLocation(m_shaderProgram, "usePointColor"), 0);
            glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1.0f, -1.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUniform3fv(glGetUniformLocation(m_shaderProgram, "objectColor"), 1,
                         glm::value_ptr(RenderConfig::WIREFRAME_OVERLAY_COLOR));
            glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
            glDisable(GL_POLYGON_OFFSET_LINE);
            break;

        case RenderMode::POINTS:
            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize(RenderConfig::POINT_SIZE);
            glUniform3fv(glGetUniformLocation(m_shaderProgram, "objectColor"), 1,
                         glm::value_ptr(RenderConfig::POINT_COLOR));
            glUniform1i(glGetUniformLocation(m_shaderProgram, "usePointColor"), 1);

            glDrawArrays(GL_POINTS, 0, m_vertexCount);

            glDisable(GL_PROGRAM_POINT_SIZE);
            break;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0);
}

void Renderer::run() {
    std::cout << "  Left Mouse + Drag  - Rotate model" << std::endl;
    std::cout << "  Mouse Wheel        - Zoom in/out" << std::endl;
    std::cout << "  1                  - Solid polygons (default)" << std::endl;
    std::cout << "  2                  - Wireframe" << std::endl;
    std::cout << "  3                  - Solid + Wireframe" << std::endl;
    std::cout << "  4                  - Points only" << std::endl;
    std::cout << "  R                  - Reset camera" << std::endl;
    std::cout << "  ESC                - Exit\n" << std::endl;

    while (!glfwWindowShouldClose(m_window)) {
        render();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Renderer::cleanup() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    if (m_shaderProgram) glDeleteProgram(m_shaderProgram);

    if (m_window) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}

void Renderer::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            renderer->m_mousePressed = true;
            glfwGetCursorPos(window, &renderer->m_lastMouseX, &renderer->m_lastMouseY);
        } else if (action == GLFW_RELEASE) {
            renderer->m_mousePressed = false;
        }
    }
}

void Renderer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

    if (renderer->m_mousePressed) {
        double deltaX = xpos - renderer->m_lastMouseX;
        double deltaY = ypos - renderer->m_lastMouseY;

        renderer->m_rotationY += static_cast<float>(deltaX) * RenderConfig::MOUSE_SENSITIVITY;
        renderer->m_rotationX += static_cast<float>(deltaY) * RenderConfig::MOUSE_SENSITIVITY;

        renderer->m_lastMouseX = xpos;
        renderer->m_lastMouseY = ypos;
    }
}

void Renderer::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

    glm::vec3 direction = renderer->m_cameraTarget - renderer->m_cameraPos;
    float distance = glm::length(direction);
    direction = glm::normalize(direction);

    distance -= static_cast<float>(yoffset) * RenderConfig::SCROLL_SENSITIVITY;
    distance = glm::max(RenderConfig::MIN_CAMERA_DISTANCE, distance);

    renderer->m_cameraPos = renderer->m_cameraTarget - direction * distance;
}

void Renderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

    switch (key) {
        case GLFW_KEY_1:
            renderer->m_renderMode = RenderMode::SOLID;
            break;
        case GLFW_KEY_2:
            renderer->m_renderMode = RenderMode::WIREFRAME;
            break;
        case GLFW_KEY_3:
            renderer->m_renderMode = RenderMode::SOLID_WITH_WIREFRAME;
            break;
        case GLFW_KEY_4:
            renderer->m_renderMode = RenderMode::POINTS;
            break;
        case GLFW_KEY_R:
            renderer->m_rotationX = 0.0f;
            renderer->m_rotationY = 0.0f;
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
    }
}
