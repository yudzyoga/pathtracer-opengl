#include "tests/test_sphere.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"
#include "vertex_buffer_layout.h"
#include <GL/gl.h>
#include <memory>

namespace test {
TestSphere::TestSphere() {
	float positions[] = {-1.f, -1.f, 0.f, 0.f, 1.f,	 -1.f, 1.f, 0.f,
						 1.f,  1.f,	 1.f, 1.f, -1.f, 1.f,  0.f, 1.f};
	unsigned int indices[] = {0, 1, 2, 2, 3, 0};

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
	std::cout << "[INFO] Compiling VertFrag ... " << std::endl;
	m_ShaderVertFrag =
		std::make_unique<Shader>("../res/shaders/sphere_vertfrag.shader");
	std::cout << "[INFO] Compiling Compute ... " << std::endl;
	m_ShaderCompute =
		std::make_unique<Shader>("../res/shaders/sphere_compute.shader");

	// * Texture
	m_Texture = std::make_unique<Texture>(SCREEN_WIDTH, SCREEN_HEIGHT);

	m_VAO->Unbind();
	m_ShaderCompute->Unbind();
	m_ShaderVertFrag->Unbind();
	m_VertexBuffer->Unbind();
	m_IndexBuffer->Unbind();
}

TestSphere::~TestSphere() {}

void TestSphere::OnUpdate(float deltaTime, unsigned int state) {}

void TestSphere::OnRender() {
	GLCall(glClearColor(0.f, 0.f, 0.f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	Renderer renderer;

	// * compute shader
	m_ShaderCompute->Bind();
	m_ShaderCompute->SetUniform3f("u_CameraPosition", m_CameraPosition.x,
								  m_CameraPosition.y, m_CameraPosition.z);
	m_ShaderCompute->SetUniform3f(
		"u_CameraRotationEuler", m_CameraRotationEuler.x,
		m_CameraRotationEuler.y, m_CameraRotationEuler.z);
	m_ShaderCompute->SetUniform1i("u_FrameIndex", m_frameCount);
	m_ShaderCompute->SetUniform1i("u_SampleOffset", m_sampleOffset);
	m_ShaderCompute->SetUniform1i("u_MaxFrameIndex", m_maxFrameCount);

	glDispatchCompute(ceil(SCREEN_WIDTH / 8), ceil(SCREEN_HEIGHT / 8), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// * vertex and fragment shader
	m_ShaderVertFrag->Bind();
	m_Texture->Bind(0);
	m_ShaderVertFrag->SetUniform1i("u_Texture", 0);

	renderer.Draw(*m_VAO, *m_IndexBuffer, *m_ShaderVertFrag);

	if (m_frameCount < m_maxFrameCount) {
		m_frameCount++;
	}
	if (m_isChanged) {
		m_sampleOffset += m_frameCount;
		m_frameCount = 0;
		m_isChanged = false;
	}
}

void TestSphere::OnImGuiRender() {
	// depth
	if (ImGui::SliderFloat3("Camera Translation", &m_CameraPosition.x, -5.0f,
							5.0f)) {
		m_isChanged = true;
	};
	if (ImGui::SliderFloat3("Camera Rotation", &m_CameraRotationEuler.x, -35.0f,
							35.0f)) {
		m_isChanged = true;
	};

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS), %d, %d",
				1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate,
				m_frameCount, m_sampleOffset);
}

} // namespace test