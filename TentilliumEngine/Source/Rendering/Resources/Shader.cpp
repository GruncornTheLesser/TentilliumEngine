#include "Shader.h"
#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <iostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <exception>
#include <gtc/type_ptr.hpp>

GLuint CompileShader(const GLenum type, std::string source)
{
	//std::cout << source << std::endl;

	GLuint shader = glCreateShader(type);
	const char* src = &source[0];

	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int compiledSuccessfully;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiledSuccessfully);
	if (!compiledSuccessfully)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shader, length, &length, message);

		glDeleteShader(shader);

		std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex shader : " : "fragment shader : ") << message << std::endl;
		throw std::exception();
	}

	return shader;
}

Shader* Resource<Shader>::Load(std::string filepath)
{
	std::ifstream stream(filepath);
	std::stringstream source;
	std::string line;
	GLenum type;

	GLuint program = glCreateProgram();

	while (getline(stream, line))
	{
		if (line[0] == '#')
		{
			std::istringstream iss(line);
			std::string command;
			iss >> command;

			if (command == "#SHADER")
			{
				if (line == "#SHADER VERTEX")		 type = GL_VERTEX_SHADER;
				else if (line == "#SHADER FRAGMENT") type = GL_FRAGMENT_SHADER;
				else if (line == "#SHADER GEOMETRY") type = GL_GEOMETRY_SHADER;
				continue;
			}
			else if (command == "#ENDSHADER")
			{
				GLuint shader = CompileShader(type, source.str());
				glAttachShader(program, shader);
				source.str(""); // empty the source
				continue;
			}
		}
		source << line << std::endl;
	}
	glLinkProgram(program);

	glValidateProgram(program);
	GLint success;

	glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
	if (!success)
	{
		int length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetProgramInfoLog(program, length, &length, &message[0]); // this doesnt work and i have no idea why???
		std::cerr << "Failed to attach shaders. \n ErrorLog: '" << message << "'" << std::endl; // the error message is gobbldegoop
		// its almost identical to the error processing above for compiling shaders
		throw std::exception();
	}

	// delete and detach shaders
	//glDetachShader(program, gs);
	//glDeleteShader(gs);

	return Resource::Emplace(filepath, program);
}

Shader::~Shader()
{
	glDeleteProgram(m_program);
	std::cout << "deleting program " << m_program << std::endl;
}

const void Shader::bind() const
{
	glUseProgram(m_program);
}

const void Shader::setUniform1i(const std::string& uniform_name, int v) const
{
	glProgramUniform1i(m_program, getUniformLocation(uniform_name), v);
}

const void Shader::setUniform1f(const std::string& uniform_name, float v) const
{
	glProgramUniform1f(m_program, getUniformLocation(uniform_name), v);
}

const void Shader::setUniform2i(const std::string& uniform_name, int v1, int v2) const
{
	glProgramUniform2i(m_program, getUniformLocation(uniform_name), v1, v2);
}

const void Shader::setUniform2i(const std::string& uniform_name, glm::i32vec2& value) const
{
	glProgramUniform2iv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

const void Shader::setUniform2f(const std::string& uniform_name, float v1, float v2) const
{
	glProgramUniform2f(m_program, getUniformLocation(uniform_name), v1, v2);
}

const void Shader::setUniform2f(const std::string& uniform_name, glm::vec2& value) const
{
	glProgramUniform2fv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

const void Shader::setUniform3i(const std::string& uniform_name, int v1, int v2, int v3) const
{
	glProgramUniform3i(m_program, getUniformLocation(uniform_name), v1, v2, v3);
}

const void Shader::setUniform3i(const std::string& uniform_name, glm::i32vec3& value) const
{
	glProgramUniform3iv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

const void Shader::setUniform3f(const std::string& uniform_name, float v1, float v2, float v3) const
{
	glProgramUniform3f(m_program, getUniformLocation(uniform_name), v1, v2, v3);
}

const void Shader::setUniform3f(const std::string& uniform_name, glm::vec3& value) const
{
	glProgramUniform3fv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

const void Shader::setUniform4i(const std::string& uniform_name, int v1, int v2, int v3, int v4) const
{
	glProgramUniform4i(m_program, getUniformLocation(uniform_name), v1, v2, v3, v4);
}

const void Shader::setUniform4i(const std::string& uniform_name, glm::i32vec4& value) const
{
	glProgramUniform4iv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

const void Shader::setUniform4f(const std::string& uniform_name, float v1, float v2, float v3, float v4) const
{
	glProgramUniform4f(m_program, getUniformLocation(uniform_name), v1, v2, v3, v4);
}

const void Shader::setUniform4f(const std::string& uniform_name, glm::vec4& value) const
{
	glProgramUniform4fv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

const void Shader::setUniformMatrix3f(const std::string& uniform_name, float value[9]) const
{
	glProgramUniformMatrix3fv(m_program, getUniformLocation(uniform_name), 1, false,&value[0]);
}

const void Shader::setUniformMatrix3f(const std::string& uniform_name, glm::mat3& value) const
{
	glProgramUniformMatrix3fv(m_program, getUniformLocation(uniform_name), 1, false,glm::value_ptr(value));
}

const void Shader::setUniformMatrix4f(const std::string& uniform_name, float value[16]) const
{
	glProgramUniformMatrix4fv(m_program, getUniformLocation(uniform_name), 1, false, &value[0]);
}

const void Shader::setUniformMatrix4f(const std::string& uniform_name, glm::mat4& value) const
{
	glProgramUniformMatrix4fv(m_program, getUniformLocation(uniform_name), 1, false, glm::value_ptr(value));
}

const void Shader::setUniformBlock(const std::string& block_name, unsigned int Binding) const
{
	glUniformBlockBinding(m_program, getUniformBlockLocation(block_name), Binding);
}

const unsigned int Shader::getUniformLocation(const std::string& uniform_name) const
{
	GLint location;
	if (uniform_cache.find(uniform_name) != uniform_cache.end())
		return uniform_cache[uniform_name];

	location = glGetUniformLocation(m_program, uniform_name.c_str());
	if (location == -1)
		std::cout << "Warning uniform '" << uniform_name << "' doesnt exist." << std::endl;
	
	uniform_cache[uniform_name] = location;
	return location;
}

const unsigned int Shader::getUniformBlockLocation(const std::string& block_name) const
{
	GLint location;
	if (block_cache.find(block_name) != block_cache.end())
		return uniform_cache[block_name];

	location = glGetUniformBlockIndex(m_program, block_name.c_str());
	if (location == -1)
		std::cout << "Warning uniform '" << block_name << "' doesnt exist." << std::endl;

	uniform_cache[block_name] = location;
	return location;
}


