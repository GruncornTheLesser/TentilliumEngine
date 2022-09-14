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
	m_size = other.m_size;
	m_size_attribute = other.m_size_attribute;

	other.m_handle = 0;
}

VAO& VAO::operator=(VAO&& other)
{
	if (m_handle)
		glDeleteVertexArrays(1, &m_handle);

	m_handle = other.m_handle;
	m_size = other.m_size;
	m_size_attribute = other.m_size_attribute;

	other.m_handle = 0;

	return *this;
}

void VAO::draw(int primitive)
{
	glBindVertexArray(m_handle);

	if (m_size_attribute == Index)
		glDrawElements(primitive, m_size, GL_UNSIGNED_INT, NULL);
	else
		glDrawArrays(primitive, 0, m_size);
}

template<> void VAO::attach<Index>(VBO<Index>* buffer)
{
	if (!buffer) return;

	glBindVertexArray(m_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->handle);

	glBindVertexArray(NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

	if (m_size_attribute > Index) {
		m_size_attribute = Index;
		m_size = buffer->get_size() / 4;
	}
}
template<> void VAO::attach<Position>(VBO<Position>* buffer) { attach(Position, buffer, 3, GL_FLOAT, false, 0); }
template<> void VAO::attach<Normal>(VBO<Normal>* buffer) { attach(Normal, buffer, 3, GL_FLOAT, false, 0); }
template<> void VAO::attach<TexCoord>(VBO<TexCoord>* buffer) { attach(TexCoord, buffer, 2, GL_FLOAT, false, 0); }
template<> void VAO::attach<BoneID>(VBO<BoneID>* buffer) { attach(BoneID, buffer, 4, GL_INT, false, 0); }
template<> void VAO::attach<BoneWeight>(VBO<BoneWeight>* buffer) { attach(BoneWeight, buffer, 4, GL_FLOAT, false, 0); }

template<> void VAO::detach<Index>()
{
	glBindVertexArray(m_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	glBindVertexArray(NULL);

	if (m_size_attribute == Index)
		findSize();

	glBindVertexArray(NULL);
}
template<> void VAO::detach<Position>() { detach(Position); }
template<> void VAO::detach<Normal>() { detach(Normal); }
template<> void VAO::detach<TexCoord>() { detach(TexCoord); }
template<> void VAO::detach<BoneID>() { detach(BoneID); }
template<> void VAO::detach<BoneWeight>() { detach(BoneWeight); }

void VAO::attach(int attrib_no, GLbuffer* buffer, int attrib_size, int type, bool normalized, int stride)
{
	if (!buffer) return;

	glBindVertexArray(m_handle);
	glBindBuffer(GL_ARRAY_BUFFER, buffer->handle);

	glVertexAttribPointer(attrib_no, attrib_size, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(attrib_no);

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	if (m_size_attribute > attrib_no) {
		m_size_attribute = (VertAttrib)attrib_no;
		m_size = buffer->get_size() / attrib_size / 4;
	}
}

void VAO::detach(int attrib_no)
{
	glBindVertexArray(m_handle);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glVertexAttribPointer(attrib_no, 0, GL_FLOAT, GL_FALSE, 0, NULL);

	if (m_size_attribute == attrib_no)
		findSize();

	glBindVertexArray(NULL);
}

void VAO::updateSize()
{
	glBindVertexArray(m_handle);
	int vbo = 0;
	glGetVertexAttribiv(m_size_attribute, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vbo);
	if (vbo == 0) {
		findSize();
		return;
	}
	int attrib_size;

	glGetNamedBufferParameteriv(vbo, GL_BUFFER_SIZE, &m_size);
	glGetVertexAttribiv(m_size_attribute, GL_VERTEX_ATTRIB_ARRAY_SIZE, &attrib_size);
		
	m_size = m_size / 4 / attrib_size;
}

void VAO::genVAO()
{
	glGenVertexArrays(1, &m_handle);
}

void VAO::findSize()
{
	m_size_attribute = None;
	m_size = 0;

	int ebo = 0;
	glGetVertexArrayiv(m_handle, GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);

	if (ebo != 0) {
		m_size_attribute = Index;
		glGetNamedBufferParameteriv(ebo, GL_BUFFER_SIZE, &m_size);
		return;
	}

	glBindVertexArray(m_handle);
	for (int i = 1; i < None; i++)
	{
		int vbo = 0;
		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vbo);
		if (vbo != 0) {
			m_size_attribute = (VertAttrib)i;
			glGetNamedBufferParameteriv(vbo, GL_BUFFER_SIZE, &m_size);
			int attrib_size;
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &attrib_size);
			m_size = m_size / 4 / attrib_size;
		}
	}
	glBindVertexArray(NULL);
}