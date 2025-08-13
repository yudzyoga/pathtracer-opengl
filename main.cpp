// #include <GL/gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include "index_buffer.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

#include "stb_image.h"
// struct ShaderProgramSource {
// 	std::string VertexSource;
// 	std::string FragmentSource;
// };

// static ShaderProgramSource ParseShader(const std::string &filepath) {
// 	// * read file
// 	std::ifstream stream(filepath);
// 	enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

// 	std::string line;
// 	std::stringstream ss[2];
// 	ShaderType type = ShaderType::NONE;
// 	while (getline(stream, line)) {
// 		if (line.find("#shader") != std::string::npos) {
// 			if (line.find("vertex") != std::string::npos)
// 				// vertex
// 				type = ShaderType::VERTEX;
// 			else if (line.find("fragment") != std::string::npos)
// 				// fragment
// 				type = ShaderType::FRAGMENT;
// 		} else {
// 			ss[(int)type] << line << "\n";
// 		}
// 	}

// 	return {ss[0].str(), ss[1].str()};
// }

// static unsigned int CompileSHader(unsigned int type,
// 								  const std::string &source) {
// 	unsigned int id = glCreateShader(type);
// 	const char *src = source.c_str();
// 	glShaderSource(id, 1, &src, nullptr);
// 	glCompileShader(id);

// 	// * error handling
// 	int result;
// 	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
// 	if (result == GL_FALSE) {
// 		int length;
// 		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
// 		char *message = (char *)alloca(length * sizeof(char));
// 		glGetShaderInfoLog(id, length, &length, message);
// 		std::cout << "Failed to compile "
// 				  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
// 				  << std::endl;
// 		std::cout << message << std::endl;
// 		glDeleteShader(id);
// 		return 0;
// 	}

// 	return id;
// }

// static unsigned int CreateShader(const std::string &vertexShader,
// 								 const std::string &fragmentShader) {
// 	unsigned int program = glCreateProgram();
// 	unsigned int vs = CompileSHader(GL_VERTEX_SHADER, vertexShader);
// 	unsigned int fs = CompileSHader(GL_FRAGMENT_SHADER, fragmentShader);

// 	glAttachShader(program, vs);
// 	glAttachShader(program, fs);
// 	glLinkProgram(program);

// 	// * performs validation on the program
// 	glValidateProgram(program);

// 	glDeleteShader(vs);
// 	glDeleteShader(fs);
// 	return program;
// }

int main() {
	GLFWwindow *window;

	// initialize the library
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(500, 500, "OpenGL is fun!", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// If using GLEW:
	if (glewInit() != GLEW_OK) {
		// handle error
	}

	{
		float positions[] = {-0.8f, -0.8f, 0.f, 0.f, 0.8f,	-0.8f, 1.f, 0.f,
							 0.8f,	0.8f,  1.f, 1.f, -0.8f, 0.8f,  0.f, 1.f};
		unsigned int indices[] = {0, 1, 2, 2, 3, 0};

		// unsigned int vao; // vertex arr obj, must use for CORE
		// GLCall(glGenVertexArrays(1, &vao));
		// GLCall(glBindVertexArray(vao));

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		// * implement VertexBuffer instead of manual mentioning
		VertexArray va;
		VertexBuffer vb(positions,
						4 * 4 * sizeof(float)); // num pt, num data per pt
		VertexBufferLayout layout;
		layout.Push<float>(2); // position
		layout.Push<float>(2); // texcoords
		va.AddBuffer(vb, layout);

		// BufferLayout layout;
		// layout.Push<float>(3);
		// va.AddLayout(layout);

		// defines that the position is consists of 2 elements only
		// GLCall(glEnableVertexAttribArray(0));
		// GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
		// 							 sizeof(float) * 2, 0));
		// when we BindVertexArray and BindBuffer, nothing actually links the
		// two. but when we specify glVertexAttribPointer, we're saying that the
		// index 0 will be bound to the currently index 0, so the
		// vertexAttribPointer links both the `buffer` and the `vao`.
		// 1. (single VAO) if we use CORE, then we need to use VAO (default).
		// what you could do, create one VAO and leave it bounds for the
		// duration of entire program. always have ONE VAO bound. then, bind
		// vertex buffer, and specify vertex layout. (past: faster)
		// 2. (multiple VAO) for every geometry, build a VAO. specify
		// glVertexAttribPointer as many times as you need, and bind vertex
		// buffer before do all that, and binds different vao every time you
		// want to draw your geometry, bind index, etc. best method: try both
		// and see the performance compares.

		// * implement IndexBuffer instead of manual mentioning just like before
		IndexBuffer ib(indices, 6);

		// const std::string filepath = ;
		// ShaderProgramSource source = ParseShader(filepath);
		// unsigned int shader =
		// 	CreateShader(source.VertexSource, source.FragmentSource);
		Shader shader("../res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

		Texture texture("../files/test.jpg");
		texture.Bind(0);
		shader.SetUniform1i("u_Texture", 0);

		va.Unbind();
		shader.Unbind();
		vb.Unbind();
		ib.Unbind();

		// unbound everything
		// GLCall(glUseProgram(0));
		// GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		// GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		float r = 0.0f;
		float increment = 0.01f;

		Renderer renderer;

		while (!glfwWindowShouldClose(window)) {
			// draw here
			// GLCall(glClear(GL_COLOR_BUFFER_BIT));
			renderer.Clear();

			// * set program and set uniform
			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
			// GLCall(glUseProgram(shader));
			// GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

			renderer.Draw(va, ib, shader);

			// * bind both vertex array and index buf
			va.Bind();
			ib.Bind();

			// * at last, call the draw elements
			// GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
			// nullptr));
			if (r > 1.0f)
				increment = -0.01f;
			else if (r < 0.0f)
				increment = 0.01f;
			r += increment;

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());
		}

		// GLCall(glDeleteProgram(shader));
	}

	GLCall(glfwTerminate());
	return 0;
}
