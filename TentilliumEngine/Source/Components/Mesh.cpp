#include "Mesh.h"
#include <glew.h>
#include <glfw3.h>

VAO::~VAO()
{
	if (m_handle)
		glDeleteVertexArrays(1, &m_handle);
}

VAO::VAO(VAO&& other)
{
	m_handle = other.m_handle;
	other.m_handle = 0;
}

VAO& VAO::operator=(VAO&& other)
{
	if (m_handle)
		glDeleteVertexArrays(1, &m_handle);

	m_handle = other.m_handle;
	other.m_handle = 0;

	return *this;
}

void VAO::draw()
{
	glBindVertexArray(m_handle);

	if (m_size != 0)
		glDrawElements(GL_TRIANGLES, m_size, GL_UNSIGNED_INT, NULL);
	else
		glDrawArrays(GL_TRIANGLES, 0, GL_UNSIGNED_INT);
}

void VAO::genVAO()
{
	glGenVertexArrays(1, &m_handle);
	glBindVertexArray(m_handle);
}

void VAO::finalize()
{
	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

void VAO::attach(VBO<VertAttrib::Index>* buffer)
{
	if (!buffer) return;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->handle);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &m_size);
	m_size /= 4;
}

void VAO::attach(VBO<VertAttrib::Position>* buffer)
{
	if (!buffer) return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
}

void VAO::attach(VBO<VertAttrib::Normal>* buffer)
{
	if (!buffer) return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
}

void VAO::attach(VBO<VertAttrib::TexCoord>* buffer)
{
	if (!buffer) return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
}

void VAO::attach(VBO<VertAttrib::BoneID>* buffer)
{
	if (!buffer) return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	glVertexAttribPointer(3, 4, GL_INT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);
}

void VAO::attach(VBO<VertAttrib::BoneWeight>* buffer)
{
	if (!buffer) return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer->handle);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(4);
}
