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
	m_size_attribute = other.m_size_attribute;

	other.m_handle = 0;
}

VAO& VAO::operator=(VAO&& other)
{
	if (m_handle)
		glDeleteVertexArrays(1, &m_handle);

	m_handle = other.m_handle;
	m_size_attribute = other.m_size_attribute;

	other.m_handle = 0;

	return *this;
}

void VAO::draw(int primitive, int size)
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

template<> void VAO::attach<V_Index>(VBO<V_Index>* buffer)
{
	if (!buffer) return;

	glBindVertexArray(m_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->handle);

	glBindVertexArray(NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

	if (m_size_attribute > V_Index) {
		m_size_attribute = V_Index;
		m_size = buffer->get_size() / 4;
	}
}
template<> void VAO::attach<V_Position>(VBO<V_Position>* buffer) { attach(V_Position, buffer, 3, GL_FLOAT, false, 0); }
template<> void VAO::attach<V_Normal>(VBO<V_Normal>* buffer) { attach(V_Normal, buffer, 3, GL_FLOAT, false, 0); }
template<> void VAO::attach<V_TexCoord>(VBO<V_TexCoord>* buffer) { attach(V_TexCoord, buffer, 2, GL_FLOAT, false, 0); }
template<> void VAO::attach<V_BoneID>(VBO<V_BoneID>* buffer) { attach(V_BoneID, buffer, 4, GL_INT, false, 0); }
template<> void VAO::attach<V_BoneWeight>(VBO<V_BoneWeight>* buffer) { attach(V_BoneWeight, buffer, 4, GL_FLOAT, false, 0); }

template<> void VAO::detach<V_Index>()
{
	glBindVertexArray(m_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	glBindVertexArray(NULL);

	if (m_size_attribute == V_Index)
		findSize();

	glBindVertexArray(NULL);
}
template<> void VAO::detach<V_Position>() { detach(V_Position); }
template<> void VAO::detach<V_Normal>() { detach(V_Normal); }
template<> void VAO::detach<V_TexCoord>() { detach(V_TexCoord); }
template<> void VAO::detach<V_BoneID>() { detach(V_BoneID); }
template<> void VAO::detach<V_BoneWeight>() { detach(V_BoneWeight); }

void VAO::attach(int attrib_no, GLbuffer* buffer, int attrib_size, int type, bool normalized, int stride)
{
	if (!buffer) return;

	glBindVertexArray(m_handle);
	glBindBuffer(GL_ARRAY_BUFFER, buffer->handle);

	glVertexAttribPointer(attrib_no, attrib_size, type, normalized, stride, NULL);
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

void VAO::genVAO()
{
	glGenVertexArrays(1, &m_handle);
}

void VAO::findSize()
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