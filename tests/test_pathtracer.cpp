#include "tests/test_pathtracer.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "renderer.h"
#include "shader.h"
#include "texture.h"
#include "vertex_buffer_layout.h"
#include <GL/gl.h>
#include <memory>

namespace test {
TestPathTracer::TestPathTracer() {
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
		std::make_unique<Shader>("../res/shaders/pt_vertfrag.shader");
	std::cout << "[INFO] Compiling Compute ... " << std::endl;
	m_ShaderCompute =
		std::make_unique<Shader>("../res/shaders/pt_compute.shader");

	glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
	glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screenTex, 1, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// * Texture
	// m_Texture = std::make_unique<Texture>(SCREEN_WIDTH, SCREEN_HEIGHT);
	// m_Texture->Bind(0);
	// m_ShaderVertFrag->SetUniform1i("u_Texture", 0);

	m_VAO->Unbind();
	m_ShaderCompute->Unbind();
	m_ShaderVertFrag->Unbind();
	m_VertexBuffer->Unbind();
	m_IndexBuffer->Unbind();
}

TestPathTracer::~TestPathTracer() {}

void TestPathTracer::OnUpdate(float deltaTime) {}

void TestPathTracer::OnRender() {
	GLCall(glClearColor(0.f, 0.f, 0.f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	Renderer renderer;

	// * compute shader
	m_ShaderCompute->Bind();
	glDispatchCompute(ceil(SCREEN_WIDTH / 8), ceil(SCREEN_HEIGHT / 4), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// * vertex and fragment shader
	m_ShaderVertFrag->Bind();
	glBindTextureUnit(0, screenTex);
	// m_Texture->Bind(0);

	m_ShaderVertFrag->SetUniform1i("u_Texture", 0);

	renderer.Draw(*m_VAO, *m_IndexBuffer, *m_ShaderVertFrag);
}

void TestPathTracer::OnImGuiRender() {}

} // namespace test