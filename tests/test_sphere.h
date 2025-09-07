#pragma once

#include "shader.h"
#include "test.h"
#include "texture.h"
#include <glm/glm.hpp>
#include <memory>

namespace test {

class TestSphere : public Test {
  public:
	TestSphere();
	~TestSphere();

	void OnUpdate(float deltaTime, unsigned int state) override;
	void OnRender() override;
	void OnImGuiRender() override;

  private:
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;

	std::unique_ptr<Shader> m_ShaderCompute, m_ShaderVertFrag;
	std::unique_ptr<Texture> m_Texture;

	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	glm::mat4 m_Proj, m_View;

	unsigned int m_frameCount = 0;
	unsigned int m_sampleOffset = 0;
	unsigned int m_maxFrameCount = 16000;
	bool m_isChanged = false;

	unsigned int SCREEN_WIDTH = 512;
	unsigned int SCREEN_HEIGHT = 512;
	glm::vec3 m_CameraPosition = glm::vec3(0.f, 5.f, -5.f);
	glm::vec3 m_CameraRotationEuler = glm::vec3(-35.f, 0.f, 0.f);
	// GLuint screenTex;
	// GLuint screenVertexShader;
	// GLuint screenShaderProgram;
	// GLuint screenFragmentShader;
	// GLuint computeShader;
	// GLuint computeProgram;
	// GLuint VAO, VBO, EBO;
};

} // namespace test