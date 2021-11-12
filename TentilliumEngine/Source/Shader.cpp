#include "Shader.h"
#include "OpenGL.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <exception>
#include <gtc/type_ptr.hpp>

struct ShaderSource
{
	bool HasVert, HasFrag, HasGeom;
	std::string Vert, Frag, Geom;
};

static ShaderSource ParseShader(const std::string& filepath)
{
	enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2 };
	ShaderType type = ShaderType::NONE;

	std::ifstream stream(filepath);
	std::string line;
	std::stringstream source[3];
	bool contains[3] = { false, false, false };
	while (getline(stream, line))
	{
		if (line.find("#shader") == std::string::npos)
			source[(int)type] << line << "\n";
		else
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
			else if (line.find("geometry") != std::string::npos)
				type = ShaderType::GEOMETRY;

			contains[(int)type] = true;
		}
	}

	return {
		contains[(int)ShaderType::VERTEX], 
		contains[(int)ShaderType::FRAGMENT], 
		contains[(int)ShaderType::GEOMETRY], 
		source[(int)ShaderType::VERTEX].str(),
		source[(int)ShaderType::FRAGMENT].str(),
		source[(int)ShaderType::GEOMETRY].str() 
	};

}

static GLuint CompileShader(const GLenum type, const std::string& source)
{
	GLuint id = glCreateShader(type);
	const char* src = &source[0];
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int compiledSuccessfully;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiledSuccessfully);
	if (!compiledSuccessfully)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		
		glDeleteShader(id);
		
		std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex shader : " : "fragment shader : ") << message << std::endl;
		throw std::exception();
	}

	return id;
}

static GLuint CreateShader(const char* filepath)
{
	ShaderSource src = ParseShader(filepath);

	GLuint program = glCreateProgram();
	GLuint vs, fs, gs;
	if (src.HasVert)
	{
		vs = CompileShader(GL_VERTEX_SHADER, src.Vert);
		glAttachShader(program, vs);
	}
	if (src.HasFrag)
	{
		fs = CompileShader(GL_FRAGMENT_SHADER, src.Frag);
		glAttachShader(program, fs);
	}
	if (src.HasGeom)
	{
		gs = CompileShader(GL_GEOMETRY_SHADER, src.Geom);
		glAttachShader(program, gs);
	}

	glLinkProgram(program);
	glValidateProgram(program);
	GLint success = 0;
	GLchar errorLog[1024] = { 0 };
	glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, sizeof(errorLog), NULL, errorLog);
		std::cout << errorLog << std::endl;
		return false;
	}
	if (src.HasFrag)
	{
		glDetachShader(program, fs);
		glDeleteShader(fs);
	}
	if (src.HasVert)
	{
		glDetachShader(program, vs);
		glDeleteShader(vs);
	}
	if (src.HasGeom)
	{
		glDetachShader(program, gs);
		glDeleteShader(gs);
	}
	return program;
}


Shader::Shader(const char* filepath) : m_program(CreateShader(filepath)) { }



Shader::~Shader()
{
	std::cout << "program would've got deleted and broken everything" << std::endl;
	//glDeleteProgram(m_program);
}


void Shader::Bind() const
{
	glUseProgram(m_program);
}


void Shader::UnBind() const
{
	glUseProgram(0);
}


void Shader::SetUniform1i(const std::string& uniform_name, int v) const
{
	glProgramUniform1i(m_program, GetUniformLocation(uniform_name), v);
}


void Shader::SetUniform1f(const std::string& uniform_name, float v) const
{
	glProgramUniform1f(m_program, GetUniformLocation(uniform_name), v);
}


void Shader::SetUniform2i(const std::string& uniform_name, int v1, int v2) const
{
	glProgramUniform2i(m_program, GetUniformLocation(uniform_name), v1, v2);
}

void Shader::SetUniform2i(const std::string& uniform_name, glm::i32vec2& value) const
{
	glProgramUniform2iv(m_program, GetUniformLocation(uniform_name), 1, glm::value_ptr(value));
}


void Shader::SetUniform2f(const std::string& uniform_name, float v1, float v2) const
{
	glProgramUniform2f(m_program, GetUniformLocation(uniform_name), v1, v2);
}

void Shader::SetUniform2f(const std::string& uniform_name, glm::vec2& value) const
{
	glProgramUniform2fv(m_program, GetUniformLocation(uniform_name), 1, glm::value_ptr(value));
}


void Shader::SetUniform3i(const std::string& uniform_name, int v1, int v2, int v3) const
{
	glProgramUniform3i(m_program, GetUniformLocation(uniform_name), v1, v2, v3);
}

void Shader::SetUniform3i(const std::string& uniform_name, glm::i32vec3& value) const
{
	glProgramUniform3iv(m_program, GetUniformLocation(uniform_name), 1, glm::value_ptr(value));
}


void Shader::SetUniform3f(const std::string& uniform_name, float v1, float v2, float v3) const
{
	glProgramUniform3f(m_program, GetUniformLocation(uniform_name), v1, v2, v3);
}

void Shader::SetUniform3f(const std::string& uniform_name, glm::vec3& value) const
{
	glProgramUniform3fv(m_program, GetUniformLocation(uniform_name), 1, glm::value_ptr(value));
}


void Shader::SetUniform4i(const std::string& uniform_name, int v1, int v2, int v3, int v4) const
{
	glProgramUniform4i(m_program, GetUniformLocation(uniform_name), v1, v2, v3, v4);
}

void Shader::SetUniform4i(const std::string& uniform_name, glm::i32vec4& value) const
{
	glProgramUniform4iv(m_program, GetUniformLocation(uniform_name), 1, glm::value_ptr(value));
}


void Shader::SetUniform4f(const std::string& uniform_name, float v1, float v2, float v3, float v4) const
{
	glProgramUniform4f(m_program, GetUniformLocation(uniform_name), v1, v2, v3, v4);
}

void Shader::SetUniform4f(const std::string& uniform_name, glm::vec4& value) const
{
	glProgramUniform4fv(m_program, GetUniformLocation(uniform_name), 1, glm::value_ptr(value));
}


void Shader::SetUniformMatrix3f(const std::string& uniform_name, float value[9]) const
{
	glProgramUniformMatrix3fv(m_program, GetUniformLocation(uniform_name), 1, false,&value[0]);
}

void Shader::SetUniformMatrix3f(const std::string& uniform_name, glm::mat3& value) const
{
	glProgramUniformMatrix3fv(m_program, GetUniformLocation(uniform_name), 1, false,glm::value_ptr(value));
}

void Shader::SetUniformMatrix4f(const std::string& uniform_name, float value[16]) const
{
	glProgramUniformMatrix4fv(m_program, GetUniformLocation(uniform_name), 1, false, &value[0]);
}

void Shader::SetUniformMatrix4f(const std::string& uniform_name, glm::mat4& value) const
{
	glProgramUniformMatrix4fv(m_program, GetUniformLocation(uniform_name), 1, false, glm::value_ptr(value));
}

void Shader::SetUniformBlock(const std::string& block_name, unsigned int Binding) const
{
	glUniformBlockBinding(m_program, GetUniformBlockLocation(block_name), Binding);
}

unsigned int Shader::GetUniformLocation(const std::string& uniform_name) const
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

unsigned int Shader::GetUniformBlockLocation(const std::string& block_name) const
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
