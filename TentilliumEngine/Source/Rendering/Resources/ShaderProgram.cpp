#include "ShaderProgram.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths
#include <gtc/type_ptr.hpp>
#include <filesystem>

template<ShaderType ... Ts>
ShaderProgram<Ts...>::ShaderProgram(std::string filepath) : Shader<Ts>(filepath)... {
	GL<ShaderProgram<Ts...>>::m_handle = glCreateProgram();

	(glAttachShader(GL<ShaderProgram<Ts...>>::m_handle, Shader<Ts>::getHandle()), ...);

	int infoLen;
	int status;

	glLinkProgram(GL<ShaderProgram<Ts...>>::m_handle);

	glGetProgramiv(GL<ShaderProgram<Ts...>>::m_handle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(GL<ShaderProgram<Ts...>>::m_handle, GL_INFO_LOG_LENGTH, &infoLen);
		char* message = (char*)alloca(infoLen * sizeof(char));
		glGetProgramInfoLog(GL<ShaderProgram<Ts...>>::m_handle, infoLen, NULL, message);

		std::cerr << "Failed to link program.\nErrorLog: '" << message << "'" << std::endl;
		throw std::exception();
	}

	glValidateProgram(GL<ShaderProgram<Ts...>>::m_handle);

	glGetProgramiv(GL<ShaderProgram<Ts...>>::m_handle, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(GL<ShaderProgram<Ts...>>::m_handle, GL_INFO_LOG_LENGTH, &infoLen);
		char* message = (char*)alloca(infoLen * sizeof(char));
		glGetProgramInfoLog(GL<ShaderProgram<Ts...>>::m_handle, infoLen, NULL, message);

		std::cerr << "Failed to validate program.\nErrorLog: '" << message << "'" << std::endl;
		throw std::exception();
	}
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::bind() const
{
	glUseProgram(GL<ShaderProgram<Ts...>>::m_handle);
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(int location, int value) const
{
	glProgramUniform1i(GL<ShaderProgram<Ts...>>::m_handle, location, value);
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(int location, float value) const
{
	glProgramUniform1f(GL<ShaderProgram<Ts...>>::m_handle, location, value);
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(int location, const glm::ivec2& value) const
{
	glProgramUniform2iv(GL<ShaderProgram<Ts...>>::m_handle, location, 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(int location, const glm::vec2& value) const
{
	glProgramUniform2fv(GL<ShaderProgram<Ts...>>::m_handle, location, 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(int location, const glm::ivec3& value) const
{
	glProgramUniform3iv(GL<ShaderProgram<Ts...>>::m_handle, location, 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(int location, const glm::vec3& value) const
{
	glProgramUniform3fv(GL<ShaderProgram<Ts...>>::m_handle, location, 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(int location, const glm::ivec4& value) const
{
	glProgramUniform4iv(GL<ShaderProgram<Ts...>>::m_handle, location, 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(int location, const glm::vec4& value) const
{
	glProgramUniform4fv(GL<ShaderProgram<Ts...>>::m_handle, location, 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(int location, const glm::mat2& value) const
{
	glProgramUniformMatrix2fv(GL<ShaderProgram<Ts...>>::m_handle, location, 1, false, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(int location, const glm::mat3& value) const
{
	glProgramUniformMatrix3fv(GL<ShaderProgram<Ts...>>::m_handle, location, 1, false, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(int location, const glm::mat4& value) const
{
	glProgramUniformMatrix4fv(GL<ShaderProgram<Ts...>>::m_handle, location, 1, false, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(std::string name, int value) const
{
	glProgramUniform1i(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), value);
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(std::string name, float value) const
{
	glProgramUniform1f(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), value);
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(std::string name, const glm::ivec2& value) const
{
	glProgramUniform2iv(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(std::string name, const glm::vec2& value) const
{
	glProgramUniform2fv(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(std::string name, const glm::ivec3& value) const
{
	glProgramUniform3iv(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(std::string name, const glm::vec3& value) const
{
	glProgramUniform3fv(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(std::string name, const glm::ivec4& value) const
{
	glProgramUniform4iv(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(std::string name, const glm::vec4& value) const
{
	glProgramUniform4fv(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), 1, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(std::string name, const glm::mat2& value) const
{
	glProgramUniformMatrix2fv(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), 1, false, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(std::string name, const glm::mat3& value) const
{
	glProgramUniformMatrix3fv(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), 1, false, glm::value_ptr(value));
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniform(std::string name, const glm::mat4& value) const
{
	glProgramUniformMatrix4fv(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), 1, false, glm::value_ptr(value));
}


template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniformBlock(int location, unsigned int Binding) const
{
	glUniformBlockBinding(GL<ShaderProgram<Ts...>>::m_handle, location, Binding);
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::setUniformBlock(std::string name, unsigned int Binding) const
{
	glUniformBlockBinding(GL<ShaderProgram<Ts...>>::m_handle, glGetUniformBlockIndex(GL<ShaderProgram<Ts...>>::m_handle, name.c_str()), Binding);
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::dispatch(glm::uvec3 workGroups) requires IsComputeProgram<Ts...>
{
	glUseProgram(GL<ShaderProgram<Ts...>>::m_handle);
	glDispatchCompute(workGroups.x, workGroups.y, workGroups.z);
}

template<ShaderType ... Ts>
unsigned int ShaderProgram<Ts...>::getLocation(std::string name) const
{
	return glGetUniformLocation(GL<ShaderProgram<Ts...>>::m_handle, name.c_str());
}

template<ShaderType ... Ts>
unsigned int ShaderProgram<Ts...>::getBlockLocation(std::string name) const
{
	return glGetUniformBlockIndex(GL<ShaderProgram<Ts...>>::m_handle, name.c_str());
}

template<ShaderType ... Ts>
void ShaderProgram<Ts...>::destroy(unsigned int handle)
{
	glDeleteProgram(handle);
}

template class ShaderProgram<COMP>;
template class ShaderProgram<VERT, FRAG>;
template class ShaderProgram<VERT, GEOM, FRAG>;

