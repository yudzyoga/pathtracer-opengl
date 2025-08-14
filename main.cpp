// #include <GL/gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
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

#include "imgui_impl_opengl3.h"

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
		float positions[] = {-100.f, -100.f, 0.f,	0.f,   100.f, -100.f,
							 1.f,	 0.f,	 100.f, 100.f, 1.f,	  1.f,
							 -100.f, 100.f,	 0.f,	1.f};
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

		glm::mat4 proj = glm::ortho(0.f, 500.f, 0.f, 500.f, -1.f, 1.f);
		glm::mat4 view =
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

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

		float r = 0.0f;
		float increment = 0.01f;

		Renderer renderer;

		// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
		// GL ES 2.0 + GLSL 100 (WebGL 1.0)
		const char *glsl_version = "#version 100";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
		// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
		const char *glsl_version = "#version 300 es";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
		// GL 3.2 + GLSL 150
		const char *glsl_version = "#version 150";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE,
					   GLFW_OPENGL_CORE_PROFILE);			 // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
		// GL 3.0 + GLSL 130
		const char *glsl_version = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// // 3.2+ only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		// // 3.0+ only
#endif

		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void)io;

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		glm::vec3 translationA(100.0f, 250.0f, 0.0f);
		glm::vec3 translationB(400.0f, 250.0f, 0.0f);
		while (!glfwWindowShouldClose(window)) {
			// draw here
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// * set program and set uniform
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va, ib, shader);
			}

			// * set program and set uniform
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va, ib, shader);
			}

			// * bind both vertex array and index buf
			va.Bind();
			ib.Bind();

			// * at last, call the draw elements
			if (r > 1.0f)
				increment = -0.01f;
			else if (r < 0.0f)
				increment = 0.01f;
			r += increment;

			{
				ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f,
									500.0f);
				ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f,
									500.0f);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
							1000.0f / io.Framerate, io.Framerate);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	GLCall(glfwTerminate());
	return 0;
}
