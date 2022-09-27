#include "ShaderProgram.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths
#include <gtc/type_ptr.hpp>
#include <filesystem>

template<ShaderType ...Ts>
ShaderProgram<Ts...>::ShaderProgram(std::string filepath) : Shader<Ts>(filepath)..., m_program(genProgram()) { }


template<ShaderType ...Ts>
ShaderProgram<Ts...>::ShaderProgram(Shader<Ts> ... shaders) : Shader<Ts>(shaders)..., m_program(genProgram()) { }

template<ShaderType ... Ts>
ShaderProgram<Ts...>::~ShaderProgram()
{
	glDeleteProgram(m_program);
}

template<ShaderType ... Ts>
ShaderProgram<Ts...>::ShaderProgram(ShaderProgram&& other) : Shader<Ts>(other)...
{
	this->m_program = other.m_program;
	other.m_program = 0;
}

template<ShaderType ... Ts>
ShaderProgram<Ts...>& ShaderProgram<Ts...>::operator=(ShaderProgram <Ts...>&& other) {
	if (this == &other)
		return *this; // call copy constructor

	glDeleteProgram(this->m_program);

	this->m_program = other.m_program;
	other.m_program = 0;
	this->m_uniform_cache.clear();

	return *this;
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::bind() const
{
	glUseProgram(m_program);
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform1i(std::string uniform_name, int v) const
{
	glProgramUniform1i(m_program, getUniformLocation(uniform_name), v);
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform1f(std::string uniform_name, float v) const
{
	glProgramUniform1f(m_program, getUniformLocation(uniform_name), v);
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform2i(std::string uniform_name, const glm::ivec2& value) const
{
	glProgramUniform2iv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform2f(std::string uniform_name, const glm::vec2& value) const
{
	glProgramUniform2fv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform3i(std::string uniform_name, const glm::ivec3& value) const
{
	glProgramUniform3iv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform3f(std::string uniform_name, const glm::vec3& value) const
{
	glProgramUniform3fv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform4i(std::string uniform_name, const glm::ivec4& value) const
{
	glProgramUniform4iv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform4f(std::string uniform_name, const glm::vec4& value) const
{
	glProgramUniform4fv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniformMat2(std::string uniform_name, const glm::mat2& value) const
{
	glProgramUniformMatrix2fv(m_program, getUniformLocation(uniform_name), 1, false, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniformMat3(std::string uniform_name, const glm::mat3& value) const
{
	glProgramUniformMatrix3fv(m_program, getUniformLocation(uniform_name), 1, false, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniformMat4(std::string uniform_name, const glm::mat4& value) const
{
	glProgramUniformMatrix4fv(m_program, getUniformLocation(uniform_name), 1, false, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniformBlock(std::string block_name, unsigned int Binding) const
{
	glUniformBlockBinding(m_program, getUniformBlockLocation(block_name), Binding);
}

template<ShaderType ...Ts>
void ShaderProgram<Ts...>::dispatch(glm::uvec3 workGroups) 
	requires is_ComputeProgram<Ts...>
{
	bind();
	glDispatchCompute(workGroups.x, workGroups.y, workGroups.z);
}

template<ShaderType ... Ts>
unsigned int ShaderProgram<Ts...>::genProgram()
{
	unsigned int program = glCreateProgram();

	(glAttachShader(program, Shader<Ts>::getHandle()), ...);

	int infoLen;
	int status;

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		char* message = (char*)alloca(infoLen * sizeof(char));
		glGetProgramInfoLog(program, infoLen, NULL, message);

		std::cerr << "Failed to link program.\nErrorLog: '" << message << "'" << std::endl;
		throw std::exception();
	}

	glValidateProgram(program);

	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		char* message = (char*)alloca(infoLen * sizeof(char));
		glGetProgramInfoLog(program, infoLen, NULL, message);

		std::cerr << "Failed to validate program.\nErrorLog: '" << message << "'" << std::endl;
		throw std::exception();
	}

	return program;
}

template<ShaderType ... Ts>
unsigned int ShaderProgram<Ts...>::getUniformLocation(std::string uniform_name) const
{
	GLint location;
	if (m_uniform_cache.find(uniform_name) != m_uniform_cache.end())
		return m_uniform_cache[uniform_name];

	location = glGetUniformLocation(m_program, uniform_name.c_str());
	if (location == -1)
		std::cerr << "Warning uniform '" << uniform_name << "' doesnt exist." << std::endl;

	m_uniform_cache[uniform_name] = location;
	return location;
}

template<ShaderType ... Ts>
unsigned int ShaderProgram<Ts...>::getUniformBlockLocation(std::string block_name) const
{
	GLint location;
	if (m_block_cache.find(block_name) != m_block_cache.end())
		return m_uniform_cache[block_name];

	location = glGetUniformBlockIndex(m_program, block_name.c_str());
	if (location == -1)
		std::cerr << "Warning uniform block '" << block_name << "' doesnt exist." << std::endl;

	m_block_cache[block_name] = location;
	return location;
}

template class ShaderProgram<COMP>;
template class ShaderProgram<VERT, FRAG>;
template class ShaderProgram<VERT, GEOM, FRAG>;

