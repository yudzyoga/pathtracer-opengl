#pragma once

#include "shader.h"
#include "test.h"
#include "texture.h"
#include <glm/glm.hpp>
#include <memory>

namespace test {

class TestPathTracer : public Test {
  public:
	TestPathTracer();
	~TestPathTracer();

	void OnUpdate(float deltaTime) override;
	void OnRender() override;
	void OnImGuiRender() override;

  private:
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;

	std::unique_ptr<Shader> m_ShaderCompute, m_ShaderVertFrag;
	// std::unique_ptr<Texture> m_Texture;

	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	glm::mat4 m_Proj, m_View;

	unsigned int frameCount = 1024;
	unsigned int SCREEN_WIDTH = 512;
	unsigned int SCREEN_HEIGHT = 512;
	GLuint screenTex;
	// GLuint screenVertexShader;
	// GLuint screenShaderProgram;
	// GLuint screenFragmentShader;
	// GLuint computeShader;
	// GLuint computeProgram;
	GLuint VAO, VBO, EBO;
};

} // namespace test