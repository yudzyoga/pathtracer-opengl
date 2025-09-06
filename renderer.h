#pragma once
#include "index_buffer.h"
#include "shader.h"
#include "vertex_array.h"
#include <GL/glew.h>

#ifdef _MSC_VER
#define DEBUG_BREAK() __debugbreak()
#else
#include <signal.h>
#define DEBUG_BREAK() raise(SIGTRAP)
#endif

#define ASSERT(x)                                                              \
	if (!(x))                                                                  \
		DEBUG_BREAK();

#define GLCall(x)                                                              \
	GLClearError();                                                            \
	x;                                                                         \
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char *function, const char *file, int line);

class Renderer {
  public:
	void Draw(const VertexArray &va, const IndexBuffer &ib,
			  const Shader &shader) const;
	// void Draw(const Shader &shader) const;

	void Clear() const;
};