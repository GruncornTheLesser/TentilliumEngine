#include "SpecularMaterial.h"
#include <glew.h>
#include <glfw3.h>

SpecularMaterial::~SpecularMaterial()
{
	glDeleteVertexArrays(1, &this->m_vao);
}

SpecularMaterial::SpecularMaterial(SpecularMaterial&& other)
{
	this->m_vao = other.m_vao;
	other.m_vao = 0;
}

SpecularMaterial& SpecularMaterial::operator=(SpecularMaterial&& other)
{
	glDeleteVertexArrays(1, &this->m_vao);

	this->m_vao = other.m_vao;
	other.m_vao = 0;

	return *this;
}

void SpecularMaterial::bind()
{
	// bind uniform buffer
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniform_buffer.handle);
	// bind diff texture
	if (m_diff_texture)
		m_diff_texture->bindSlot(0);
	// bind spec texture
	if (m_spec_texture)
		m_spec_texture->bindSlot(1);
	// bind glos texture
	if (m_glos_texture)
		m_glos_texture->bindSlot(2);
}
