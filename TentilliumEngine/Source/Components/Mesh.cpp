#include "Mesh.h"
#include <glew.h>
#include <glfw3.h>

Mesh::VAO::VAO()
{
	glGenVertexArrays(1, &m_handle);
}

void Mesh::VAO::draw(int primitive, int size) const
{
	if (size == 0)
		size = m_size;

	glBindVertexArray(m_handle);
	
	if (m_size_attribute == V_Index)
		glDrawElements(primitive, size, GL_UNSIGNED_INT, NULL);
	else
		glDrawArrays(primitive, 0, size);

	glBindVertexArray(NULL);
}

void Mesh::VAO::attach(int attrib_no, const Buffer& buffer, int attrib_size, int type, bool normalized, int stride)
{
	if (attrib_no == V_Index)
	{
		glBindVertexArray(m_handle);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.handle);

		glBindVertexArray(NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	}
	else
	{
		glBindVertexArray(m_handle);
		glBindBuffer(GL_ARRAY_BUFFER, buffer.handle);

		glVertexAttribPointer(attrib_no, attrib_size, type, normalized, stride, NULL);
		glEnableVertexAttribArray(attrib_no);

		glBindVertexArray(NULL);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}
	
	if (m_size_attribute > attrib_no) {
		m_size_attribute = (VertAttrib)attrib_no;
		m_size = buffer.length() / attrib_size / 4;
	}
}

void Mesh::VAO::detach(int attrib_no)
{
	glBindVertexArray(m_handle);
	if (attrib_no == V_Index) 
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glVertexAttribPointer(attrib_no, 0, GL_FLOAT, GL_FALSE, 0, NULL);
	}

	if (m_size_attribute == attrib_no)
		findSize();

	glBindVertexArray(NULL);
}

void Mesh::VAO::findSize()
{
	int bo, attrib_size, attribute = V_None;
	glBindVertexArray(m_handle);

	glGetVertexArrayiv(m_handle, GL_ELEMENT_ARRAY_BUFFER_BINDING, &bo);
	if (bo != 0) {
		m_size_attribute = V_Index;
		attrib_size = 1;
	}
	else 
	{
		for (int i = 0; i < V_None; i++)
		{
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &bo);
			if (bo != 0) {
				m_size_attribute = (VertAttrib)i;
				glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &attrib_size);
				break;
			}
		}
	}
	glGetNamedBufferParameteriv(bo, GL_BUFFER_SIZE, &m_size);
	glBindVertexArray(NULL);

	m_size = m_size / 4 / attrib_size;
}

void Mesh::VAO::destroy(unsigned int handle) {
	glDeleteVertexArrays(1, &handle);
}