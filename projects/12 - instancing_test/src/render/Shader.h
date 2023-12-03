#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
public:
    unsigned int renderer_id{};
    std::string file_path;
    std::unordered_map<std::string, int> m_UniformLocationCache;

    explicit Shader(const std::string &filepath) {
        file_path = filepath;
        ShaderProgramSource source = parse_shader(filepath);
        renderer_id = create_shader(source.VertexSource, source.FragmentSource);
    }

    void terminate() const {
        glDeleteProgram(renderer_id);
    }

    void bind() const {
        glUseProgram(renderer_id);
    }

    static void unbind() {
        glUseProgram(0);
    }

    void set_uniform_1i(const std::string &name, int value) {
        glUniform1i(get_uniform_location(name), value);
    }

    void set_uniform_1f(const std::string &name, float value) {
        glUniform1f(get_uniform_location(name), value);
    }

    void set_uniform_4f(const std::string &name, float v0, float v1, float v2, float v3) {
        glUniform4f(get_uniform_location(name), v0, v1, v2, v3);
    }

    void set_uniform_matrix_4fv(const std::string &name, glm::mat4 mat4) {
        glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &mat4[0][0]);
    }

    void set_uniform_3f(const std::string &name, glm::vec3 vec3) {
        glUniform3f(get_uniform_location(name), vec3.x, vec3.y, vec3.z);
    }

    static ShaderProgramSource parse_shader(const std::string &filepath) {
        std::ifstream stream(filepath);

        enum class ShaderType {
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;

        while (getline(stream, line)) {
            if (line.find("// shader") != std::string::npos) {
                if (line.find("vertex") != std::string::npos) {
                    type = ShaderType::VERTEX;
                } else if (line.find("fragment") != std::string::npos) {
                    type = ShaderType::FRAGMENT;

                }
            } else {
                ss[(int) type] << line << '\n';
            }
        }

        return {ss[0].str(), ss[1].str()};
    }

    static unsigned int compile_shader(unsigned int type, const std::string &source) {
        unsigned int id = glCreateShader(type);
        const char *src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char *message = (char *) alloca(length * sizeof(char));
            glGetShaderInfoLog(id, length, &length, message);
            glDeleteShader(id);
            return 0;
        }

        return id;
    }

    static unsigned int create_shader(const std::string &vertexShader, const std::string &fragmentShader) {
        unsigned int program = glCreateProgram();
        unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertexShader);
        unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragmentShader);

        glAttachShader(program, vs);
        glAttachShader(program, fs);

        glLinkProgram(program);
        glValidateProgram(program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }

    int get_uniform_location(const std::string &name) {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
            return m_UniformLocationCache[name];
        }
        int location = glGetUniformLocation(renderer_id, name.c_str());
        m_UniformLocationCache[name] = location;
        return location;
    }
};
