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
	// : m_Proj(glm::ortho(-1.1f, 1.1f, -1.1f, 1.1f, -1.f, 1.f)),
	//   m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))) {
	// float positions[] = {-1.f, -1.f, 0.f, 0.f, 1.f,	 -1.f, 1.f, 0.f,
	// 					 1.f,  1.f,	 1.f, 1.f, -1.f, 1.f,  0.f, 1.f};
	// unsigned int indices[] = {0, 1, 2, 2, 3, 0};

	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// // * Array
	// m_VAO = std::make_unique<VertexArray>();
	// m_VertexBuffer =
	// 	std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
	// VertexBufferLayout layout;
	// layout.Push<float>(2); // position
	// layout.Push<float>(2); // texcoords
	// m_VAO->AddBuffer(*m_VertexBuffer, layout);

	// // * Index
	// m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

	// // * Screen Texture
	// glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
	// glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// glTextureStorage2D(screenTex, 1, GL_RGBA32F, width, height);
	// glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY,
	// GL_RGBA32F);

	// // * Shader
	// std::cout << "[INFO] Compiling VertFrag ... " << std::endl;
	// m_ShaderVertFrag =
	// 	std::make_unique<Shader>("../res/shaders/pt_vertfrag.shader");
	// std::cout << "[INFO] Compiling Compute ... " << std::endl;
	// m_ShaderCompute =
	// 	std::make_unique<Shader>("../res/shaders/pt_compute.shader");

	// // * Texture
	// // m_Texture = std::make_unique<Texture>(width, height);
	// // m_Texture = std::make_unique<Texture>("../files/test2.jpg");
	// // m_Texture->Bind(0);
	// // m_Shader_VertFrag->SetUniform1i("u_Texture", 0);

	// m_VAO->Unbind();
	// m_ShaderCompute->Unbind();
	// m_ShaderVertFrag->Unbind();
	// m_VertexBuffer->Unbind();
	// m_IndexBuffer->Unbind();

	// *
	// *
	// *
	// *
	// *
	// *

	GLfloat vertices[] = {-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, -1.0f, 1.0f,
						  0.0f,	 0.0f,	1.0f,  1.0f, 1.0f, 0.0f,  1.0f,
						  1.0f,	 1.0f,	-1.0f, 0.0f, 1.0f, 0.0f};

	GLuint indices[] = {0, 2, 1, 0, 3, 2};

	const char *screenVertexShaderSource = R"(#version 460 core
	layout (location = 0) in vec3 pos;
	layout (location = 1) in vec2 uvs;
	out vec2 UVs;
	void main()
	{
		gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
		UVs = uvs;
	})";
	const char *screenFragmentShaderSource = R"(#version 460 core
	out vec4 FragColor;
	uniform sampler2D screen;
	in vec2 UVs;
	void main()
	{
		FragColor = texture(screen, UVs);
	})";
	const char *screenComputeShaderSource = R"(#version 460 core
	layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
	layout(rgba32f, binding = 0) uniform image2D screen;
	void main()
	{
		vec4 pixel = vec4(0.075, 0.133, 0.173, 1.0);
		ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
		
		ivec2 dims = imageSize(screen);
		float x = -(float(pixel_coords.x * 2 - dims.x) / dims.x); // transforms to [-1.0, 1.0]
		float y = -(float(pixel_coords.y * 2 - dims.y) / dims.y); // transforms to [-1.0, 1.0]

		float fov = 90.0;
		vec3 cam_o = vec3(0.0, 0.0, -tan(fov / 2.0));
		vec3 ray_o = vec3(x, y, 0.0);
		vec3 ray_d = normalize(ray_o - cam_o);

		vec3 sphere_c = vec3(0.0, 0.0, -5.0);
		float sphere_r = 1.0;

		vec3 o_c = ray_o - sphere_c;
		float b = dot(ray_d, o_c);
		float c = dot(o_c, o_c) - sphere_r * sphere_r;
		float intersectionState = b * b - c;
		vec3 intersection = ray_o + ray_d * (-b + sqrt(b * b - c));

		if (intersectionState >= 0.0)
		{
			pixel = vec4((normalize(intersection - sphere_c) + 1.0) / 2.0, 1.0);
		}

		imageStore(screen, pixel_coords, pixel);
	})";

	// // * Shader
	// std::cout << "[INFO] Compiling VertFrag ... " << std::endl;
	// m_ShaderVertFrag =
	// 	std::make_unique<Shader>("../res/shaders/pt_vertfrag.shader");
	// std::cout << "[INFO] Compiling Compute ... " << std::endl;
	// m_ShaderCompute =
	// 	std::make_unique<Shader>("../res/shaders/pt_compute.shader");

	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);

	glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE,
							  3 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);

	glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
	glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screenTex, 1, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	screenVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(screenVertexShader, 1, &screenVertexShaderSource, NULL);
	glCompileShader(screenVertexShader);
	screenFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(screenFragmentShader, 1, &screenFragmentShaderSource, NULL);
	glCompileShader(screenFragmentShader);

	screenShaderProgram = glCreateProgram();
	glAttachShader(screenShaderProgram, screenVertexShader);
	glAttachShader(screenShaderProgram, screenFragmentShader);
	glLinkProgram(screenShaderProgram);

	glDeleteShader(screenVertexShader);
	glDeleteShader(screenFragmentShader);

	computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &screenComputeShaderSource, NULL);
	glCompileShader(computeShader);

	computeProgram = glCreateProgram();
	glAttachShader(computeProgram, computeShader);
	glLinkProgram(computeProgram);

	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	std::cout << "Max work groups per compute shader"
			  << " x:" << work_grp_cnt[0] << " y:" << work_grp_cnt[1]
			  << " z:" << work_grp_cnt[2] << "\n";

	int work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	std::cout << "Max work group sizes"
			  << " x:" << work_grp_size[0] << " y:" << work_grp_size[1]
			  << " z:" << work_grp_size[2] << "\n";

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	std::cout << "Max invocations count per work group: " << work_grp_inv
			  << "\n";
}

TestPathTracer::~TestPathTracer() {}

void TestPathTracer::OnUpdate(float deltaTime) {}

void TestPathTracer::OnRender() {
	GLCall(glClearColor(0.f, 1.f, 0.f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	// * compute shader
	// m_ShaderCompute->Bind();
	// GLCall(glDispatchCompute(width, height, 1));
	// GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
	// m_ShaderCompute->Unbind();

	// // * vertfrag shader
	// m_ShaderVertFrag->Bind();
	// GLCall(glBindTextureUnit(0, screenTex));
	// m_ShaderVertFrag->SetUniform1i("u_Texture", 0);
	// m_VAO->Bind();
	// m_ShaderVertFrag->Unbind();
	// GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

	glUseProgram(computeProgram);
	// glDispatchCompute(ceil(SCREEN_WIDTH / 8), ceil(SCREEN_HEIGHT / 4), 1);
	glDispatchCompute(SCREEN_WIDTH, SCREEN_HEIGHT, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glUseProgram(screenShaderProgram);
	glBindTextureUnit(0, screenTex);
	glUniform1i(glGetUniformLocation(screenShaderProgram, "screen"), 0);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void TestPathTracer::OnImGuiRender() {}

} // namespace test