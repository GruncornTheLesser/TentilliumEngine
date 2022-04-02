#include "Shader.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths
#include <fstream>
#include <exception>
#include <iostream>


internal::Shader::~Shader()
{
	glDeleteShader(m_shader);
}

internal::Shader::Shader(const Shader&& other)
{
	this->m_shader = other.m_shader;
}

internal::Shader& internal::Shader::operator=(const Shader&& other)
{
	glDeleteShader(m_shader);
	this->m_shader = other.m_shader;
	return *this;
}

internal::Shader::Shader(ShaderType type, std::string text)
{
	std::string data;
	
	std::ifstream fs(text, std::ios::in | std::ios::binary | std::ios::ate);
	const int m_size = fs.tellg();	// get size
	fs.seekg(0, std::ios::beg);		// put cursor to beginning

	// get filedata
	data = std::string(m_size, ' ');
	fs.read(&data[0], m_size);
	fs.close();

	// create opengl object shader
	switch (type) {
	case ShaderType::VERT: m_shader = glCreateShader(GL_VERTEX_SHADER);		break;
	case ShaderType::GEOM: m_shader = glCreateShader(GL_GEOMETRY_SHADER);	break;
	case ShaderType::FRAG: m_shader = glCreateShader(GL_FRAGMENT_SHADER);	break;
	case ShaderType::COMP: m_shader = glCreateShader(GL_COMPUTE_SHADER);	break;
	}

	// add data to opengl object
	const GLchar* raw_data = (const GLchar*)data.c_str();
	glShaderSource(m_shader, 1, &raw_data, &m_size);

	// compile shader
	glCompileShader(m_shader);
	
	// verify shader status
	int status, infoLen;
	glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &infoLen);
		char* message = (char*)alloca(infoLen * sizeof(char));
		glGetShaderInfoLog(m_shader, infoLen, &infoLen, message);

		std::cerr << "Failed to compile shader : " << message << std::endl;

		throw std::exception();
	}
}