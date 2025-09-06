#include "shader.h"

#include "renderer.h"
#include <GL/gl.h>
#include <cstdio>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

Shader::Shader(const std::string &filepath)
	: m_FilePath(filepath), m_RendererID(0) {
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource,
								source.ComputeSource);
}

Shader::~Shader() { GLCall(glDeleteProgram(m_RendererID)); }

ShaderProgramSource Shader::ParseShader(const std::string &filepath) {
	// * read file
	std::ifstream stream(filepath);
	enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1, COMPUTE = 2 };

	std::string line;
	std::stringstream ss[3];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
				// vertex
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				// fragment
				type = ShaderType::FRAGMENT;
			else if (line.find("compute") != std::string::npos)
				// compute
				type = ShaderType::COMPUTE;
		} else {
			ss[(int)type] << line << "\n";
		}
	}

	return {ss[0].str(), ss[1].str(), ss[2].str()};
}

unsigned int Shader::CompileShader(unsigned int type,
								   const std::string &source) {
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// * error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char *)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile "
				  << (type == GL_VERTEX_SHADER
						  ? "vertex"
						  : (type == GL_FRAGMENT_SHADER ? "fragment"
														: "compute"))
				  << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string &vertexShader,
								  const std::string &fragmentShader,
								  const std::string &computeShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	unsigned int cs = CompileShader(GL_COMPUTE_SHADER, computeShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glAttachShader(program, cs);
	glLinkProgram(program);

	// * performs validation on the program
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(cs);
	return program;
}

void Shader::Bind() const { GLCall(glUseProgram(m_RendererID)); }

void Shader::Unbind() const { GLCall(glUseProgram(0)); }

// Set uniforms
void Shader::SetUniform1i(const std::string &name, int value) {
	GLCall(glUniform1i(GetUniformLocation(name), value));
}
void Shader::SetUniform1f(const std::string &name, float value) {
	GLCall(glUniform1f(GetUniformLocation(name), value));
}
void Shader::SetUniform2f(const std::string &name, float v0, float v1) {
	GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2,
						  float v3) {
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 &matrix) {
	GLCall(glUniformMatrix4fv(
		GetUniformLocation(name), 1, GL_FALSE,
		&matrix[0][0])); // get 1st elem, other gets sequentially
}

GLint Shader::GetUniformLocation(const std::string &name) const {
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(GLint location = glGetUniformLocation(m_RendererID, name.c_str()));
	m_UniformLocationCache[name] = location;
	return location;
}