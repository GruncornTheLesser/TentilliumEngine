#pragma once
#include <GLM/glm.hpp>
#include <GL/glew.h>	// extension wrangler
#include <GLFW/glfw3.h> // framework
#include <iostream>
#include <vector>




namespace gl
{
	struct Vertex {
		glm::vec3 p;
		glm::vec3 n;
		glm::vec2 t;
	};

	template<GLenum target, class T>
	class Buffer
	{
	public:
		unsigned int m_handle;
		unsigned int m_length;
	public:
		Buffer(const std::vector<T>& data)
			: m_length(data.size() * sizeof(T))
		{
			glGenBuffers(1, &m_handle);
			glBindBuffer(target, m_handle);
			glBufferData(target, m_length, &data[0], GL_STATIC_DRAW);
			glBindBuffer(target, 0);
		}

		Buffer()
			: m_handle(0), m_length(0)
		{ }
		Buffer(const Buffer& other)
			: m_handle(other.m_handle), m_length(other.m_length) { }
		Buffer& operator=(const Buffer& other)
		{
			m_handle = other.m_handle;
			m_length = other.m_length;
			return *this;
		}
		
		

		~Buffer()
		{
			if (m_handle != 0) std::cout << "buffer would've got deleted and broken everything" << std::endl;
			if (m_handle == 0) std::cout << "an unused buffer got deleted. this is fine." << std::endl;
			//glDeleteBuffers(1, &m_handle);
		}

		void Bind()
		{
			glBindBuffer(target, m_handle);
		}

		void UnBind()
		{
			glBindBuffer(target, 0);
		}

		std::vector<T> Read()
		{
			std::vector<T> data(m_length / sizeof(T));
			glGetNamedBufferSubData(m_handle, 0, m_length, &data[0]);
			return data;
		}

		void Write(std::vector<T> data, GLenum hint = GL_STATIC_DRAW)
		{
			m_length = data.size() * sizeof(T);
			glNamedBufferData(m_handle, m_length, &data[0], hint);
		}
	};
}
