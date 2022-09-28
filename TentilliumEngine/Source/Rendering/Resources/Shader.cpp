#include "Shader.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths
#include <fstream>
#include <exception>
#include <iostream>

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
	else if (!((type == VERT && fileExt == ".vert") || (type == GEOM && fileExt == ".geom") || (type == FRAG && fileExt == ".frag") || (type == COMP && fileExt == ".comp")))
	{
		std::cerr << "[Load error] - file extension does not match shader type. Type is '";
		switch (type) { 
		case VERT: std::cerr << "Vertex"; break;
		case GEOM: std::cerr << "Geometry"; break;
		case FRAG: std::cerr << "Fragment"; break;
		case COMP: std::cerr << "Compute"; break;
		}
		std::cerr << "' in file '" << filepath << "'" << std::endl;

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
	GL<Shader<type>>::m_handle = glCreateShader(type);

	// add data to opengl object
	const char* raw_data = (const char*)data.c_str();
	glShaderSource(GL<Shader<type>>::m_handle, 1, &raw_data, &len);

	// compile shader
	glCompileShader(GL<Shader<type>>::m_handle);

	// verify shader status
	int status, infoLen;
	glGetShaderiv(GL<Shader<type>>::m_handle, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		glGetShaderiv(GL<Shader<type>>::m_handle, GL_INFO_LOG_LENGTH, &infoLen);
		char* message = (char*)alloca(infoLen * sizeof(char));
		glGetShaderInfoLog(GL<Shader<type>>::m_handle, infoLen, &infoLen, message);

		std::cerr << "[Shader Error] - Shader '" << filepath << "' failed to compile: " << message << std::endl;
		throw std::exception();
	}
}

template<ShaderType type>
void Shader<type>::destroy(unsigned int handle)
{
	glDeleteShader(handle);
}

template class Shader<ShaderType::COMP>;
template class Shader<ShaderType::VERT>;
template class Shader<ShaderType::GEOM>;
template class Shader<ShaderType::FRAG>;