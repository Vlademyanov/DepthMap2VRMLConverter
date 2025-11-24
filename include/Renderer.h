#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

class Mesh;

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool initialize(int width, int height, const std::string& title);

    void loadMesh(const Mesh& mesh);

    void run();

    void cleanup();

private:
    GLFWwindow* m_window;

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    GLuint m_shaderProgram;


    unsigned int m_indexCount;
    unsigned int m_vertexCount;

    enum class RenderMode {
        SOLID,
        WIREFRAME,
        SOLID_WITH_WIREFRAME,
        POINTS
    };
    RenderMode m_renderMode;

    glm::vec3 m_cameraPos;
    glm::vec3 m_cameraTarget;
    float m_rotationX;
    float m_rotationY;
    double m_lastMouseX;
    double m_lastMouseY;
    bool m_mousePressed;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    bool createShaders();
    void render();
};
