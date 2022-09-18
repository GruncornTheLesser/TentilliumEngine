#include "Shader.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths
#include <fstream>
#include <exception>
#include <iostream>

template<ShaderType type>
Shader<type>::~Shader()
{
	if (Resource<Shader<type>>::destroy(m_handle))
		glDeleteShader(m_handle);
}

template<ShaderType type>
Shader<type>::Shader(const Shader<type>& other)
{
	Resource<Shader<type>>::create(other.m_handle);
	if (Resource<Shader<type>>::destroy(m_handle))
		glDeleteShader(m_handle);
	this->m_handle = other.m_handle;
}
template<ShaderType type>
Shader<type>& Shader<type>::operator=(const Shader<type>& other)
{
	if (this == &other)
		return *this;

	Resource<Shader<type>>::create(other.m_handle);
	if (Resource<Shader<type>>::destroy(m_handle))
		glDeleteShader(m_handle);

	this->m_handle = other.m_handle;

	return *this;
}

template<ShaderType type>
Shader<type>::Shader(std::string filepath)
{
	size_t it1 = filepath.find_last_of(".");
	size_t it2 = filepath.length();

	std::string fileExt = filepath.substr(it1, it2 - it1);

	if (fileExt == ".shader")
	{
		switch (type) {
		case VERT: filepath.replace(it1, it2, ".vert"); break;
		case GEOM: filepath.replace(it1, it2, ".geom"); break;
		case FRAG: filepath.replace(it1, it2, ".frag"); break;
		case COMP: filepath.replace(it1, it2, ".comp"); break;
		}
	}

	std::string data;

	std::ifstream fs(filepath, std::ios::in | std::ios::binary | std::ios::ate);
	const int len = fs.tellg();	// get size

	if (len < 1)
	{
		std::cerr << "[Load Error] - failed to load Shader from file '" << filepath << "'" << std::endl;
		throw std::exception();
	}

	fs.seekg(0, std::ios::beg);		// put cursor to beginning

	// get filedata
	data = std::string(len, ' ');
	fs.read(&data[0], len);
	fs.close();

	// create opengl object shader
	switch (type) {
	case ShaderType::VERT: m_handle = glCreateShader(GL_VERTEX_SHADER);		break;
	case ShaderType::GEOM: m_handle = glCreateShader(GL_GEOMETRY_SHADER);	break;
	case ShaderType::FRAG: m_handle = glCreateShader(GL_FRAGMENT_SHADER);	break;
	case ShaderType::COMP: m_handle = glCreateShader(GL_COMPUTE_SHADER);	break;
	}

	// add data to opengl object
	const char* raw_data = (const char*)data.c_str();
	glShaderSource(m_handle, 1, &raw_data, &len);

	// compile shader
	glCompileShader(m_handle);

	// verify shader status
	int status, infoLen;
	glGetShaderiv(m_handle, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &infoLen);
		char* message = (char*)alloca(infoLen * sizeof(char));
		glGetShaderInfoLog(m_handle, infoLen, &infoLen, message);

		std::cerr << "[Shader Error] - Shader '" << filepath << "' failed to compile: " << message << std::endl;
		throw std::exception();
	}

	Resource<Shader<type>>::create(m_handle);
}

template class Shader<ShaderType::COMP>;
template class Shader<ShaderType::VERT>;
template class Shader<ShaderType::GEOM>;
template class Shader<ShaderType::FRAG>;