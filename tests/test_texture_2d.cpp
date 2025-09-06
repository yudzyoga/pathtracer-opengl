#include "test_texture_2d.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include "index_buffer.h"
#include "renderer.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include <GL/glew.h>
#include <memory>

namespace test {
TestTexture2D::TestTexture2D()
	: m_Proj(glm::ortho(0.f, 500.f, 0.f, 500.f, -1.f, 1.f)),
	  m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
	  m_TranslationA(100, 250, 0), m_TranslationB(400, 250, 0) {

	float positions[] = {-100.f, -100.f, 0.f, 0.f, 100.f,  -100.f, 1.f, 0.f,
						 100.f,	 100.f,	 1.f, 1.f, -100.f, 100.f,  0.f, 1.f};
	unsigned int indices[] = {0, 1, 2, 2, 3, 0};

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// * Array
	m_VAO = std::make_unique<VertexArray>();
	m_VertexBuffer =
		std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
	VertexBufferLayout layout;
	layout.Push<float>(2); // position
	layout.Push<float>(2); // texcoords
	m_VAO->AddBuffer(*m_VertexBuffer, layout);

	// * Index
	m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

	// * Shader
	m_Shader = std::make_unique<Shader>("../res/shaders/Basic.shader");
	m_Shader->Bind();
	m_Shader->SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

	// * Texture
	m_Texture = std::make_unique<Texture>("../files/test2.jpg");
	m_Texture->Bind(0);
	m_Shader->SetUniform1i("u_Texture", 0);

	m_VAO->Unbind();
	m_Shader->Unbind();
	m_VertexBuffer->Unbind();
	m_IndexBuffer->Unbind();
}
TestTexture2D::~TestTexture2D() {}

void TestTexture2D::OnUpdate(float deltaTime) {}

void TestTexture2D::OnRender() {
	GLCall(glClearColor(0.f, 0.f, 0.f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	Renderer renderer;

	m_Texture->Bind(0);

	// * set program and set uniform
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
		glm::mat4 mvp = m_Proj * m_View * model;
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", mvp);
		renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}

	// * set program and set uniform
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
		glm::mat4 mvp = m_Proj * m_View * model;
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", mvp);
		renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}
}

void TestTexture2D::OnImGuiRender() {

	ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 500.0f);
	ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0.0f, 500.0f);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
} // namespace test