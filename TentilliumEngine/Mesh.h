#pragma once
#include "OpenGL.h"
class Mesh
{
private:
	int m_primitive;

	gl::Buffer<GL_ELEMENT_ARRAY_BUFFER, unsigned int>*	m_ibo;
	gl::Buffer<GL_ARRAY_BUFFER, gl::Vertex>*			m_vbo;
	unsigned int m_vao;
	
public:
	Mesh(const char* filepath, int primitive);
	void Draw();
};


