#pragma once

#include <GL/glew.h>
#include <string>

class ShaderLoader {
public:
    static GLuint loadProgram(const std::string& vertexPath, const std::string& fragmentPath);

private:
    static std::string readFile(const std::string& filepath);
    static GLuint compileShader(GLenum type, const std::string& source);
    static GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
};