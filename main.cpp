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
#include "tests/test.h"
#include "texture.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui_impl_opengl3.h"

#include "tests/testClearColor.h"

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

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

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

		test::Test *currentTest = nullptr;
		test::TestMenu *testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		testMenu->RegisterTest<test::TestClearColor>("Clear Color");

		while (!glfwWindowShouldClose(window)) {
			// draw here
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (currentTest) {
				currentTest->OnUpdate(0.0f);
				currentTest->OnRender();
				ImGui::Begin("Test");
				if (currentTest != testMenu && ImGui::Button("<-")) {
					delete currentTest;
					currentTest = testMenu;
				}
				currentTest->OnImGuiRender();
				ImGui::End();
			}

			// test.OnImGuiRender();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());
		}
		delete currentTest;
		if (currentTest != testMenu)
			delete currentTest;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	GLCall(glfwTerminate());
	return 0;
}
