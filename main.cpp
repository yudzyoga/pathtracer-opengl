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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

		// * implement IndexBuffer instead of manual mentioning just like before
		IndexBuffer ib(indices, 6);

		glm::mat4 proj = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);

		Shader shader("../res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_MVP", proj);

		Texture texture("../files/test.jpg");
		texture.Bind(0);
		shader.SetUniform1i("u_Texture", 0);

		va.Unbind();
		shader.Unbind();
		vb.Unbind();
		ib.Unbind();

		float r = 0.0f;
		float increment = 0.01f;

		Renderer renderer;

		while (!glfwWindowShouldClose(window)) {
			// draw here
			renderer.Clear();

			// * set program and set uniform
			// shader.Bind();
			// shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
			renderer.Draw(va, ib, shader);

			// * bind both vertex array and index buf
			va.Bind();
			ib.Bind();

			// * at last, call the draw elements
			if (r > 1.0f)
				increment = -0.01f;
			else if (r < 0.0f)
				increment = 0.01f;
			r += increment;

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());
		}
	}

	GLCall(glfwTerminate());
	return 0;
}
