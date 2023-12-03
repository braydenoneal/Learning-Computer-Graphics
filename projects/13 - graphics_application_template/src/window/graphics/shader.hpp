#pragma once

#include <string>
#include <fstream>
#include <sstream>

namespace shader {
    std::string parse_shader(const char *file_path) {
        std::ifstream stream(file_path);
        std::stringstream string_stream;
        string_stream << stream.rdbuf();
        return string_stream.str();
    }

    GLuint create_shaders(const char *vertex_shader_file_path, const char *fragment_shader_file_path) {
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        std::string vertex_shader_source = parse_shader(vertex_shader_file_path);
        const GLchar *vertex_source_char = vertex_shader_source.c_str();
        glShaderSource(vertex_shader, 1, &vertex_source_char, nullptr);
        glCompileShader(vertex_shader);

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fragment_shader_source = parse_shader(fragment_shader_file_path);
        const GLchar *fragment_source_char = fragment_shader_source.c_str();
        glShaderSource(fragment_shader, 1, &fragment_source_char, nullptr);
        glCompileShader(fragment_shader);

        GLuint program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);

        glLinkProgram(program);
        glValidateProgram(program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return program;
    }

    void bind(GLuint program) {
        glUseProgram(program);
    }
}
