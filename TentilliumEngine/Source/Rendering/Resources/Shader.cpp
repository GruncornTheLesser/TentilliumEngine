#include "Shader.h"
#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <fstream>
#include <exception>

internal::Shader::~Shader()
{
	glDeleteShader(m_shader);
}

internal::Shader::Shader(ShaderType type, std::string text, loadType method)
{

	std::string data;
	
	switch (method) {
		case loadType::FROM_FILE: {
			std::ifstream fs(text, std::ios::in | std::ios::binary | std::ios::ate);
			int size = fs.tellg();	// get size
			fs.seekg(0, std::ios::beg);	// put cursor to beginning

			// get filedata
			data = std::string(size, ' ');
			fs.read(&data[0], size);
			fs.close();
		
			break;
		}

		case loadType::FROM_TEXT: {
			data = text;
			break;
		}
	}

	switch (type) {
	case ShaderType::VERT: m_shader = glCreateShader(GL_VERTEX_SHADER); break;
	case ShaderType::GEOM: m_shader = glCreateShader(GL_GEOMETRY_SHADER); break;
	case ShaderType::FRAG: m_shader = glCreateShader(GL_FRAGMENT_SHADER); break;
	case ShaderType::COMP: m_shader = glCreateShader(GL_COMPUTE_SHADER); break;
	}

	const int raw_size = data.size();
	const GLchar* raw_data = (const GLchar*)data.c_str();

	glShaderSource(m_shader, 1, &raw_data, &raw_size);
	glCompileShader(m_shader);
	
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