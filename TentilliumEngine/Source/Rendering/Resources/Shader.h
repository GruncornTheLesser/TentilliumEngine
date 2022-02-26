#pragma once
#include "Resource.h"
#include <unordered_map>
#include <string>
#include <glm.hpp>
#include <iostream>

struct Shader : Resource<Shader>
{
private:
	unsigned int m_program;
	mutable std::unordered_map<std::string, unsigned int> uniform_cache;	// uniform locations
	mutable std::unordered_map<std::string, unsigned int> block_cache;		// uniform block locations

	
public:
	Shader(unsigned int prg) : m_program(prg) { std::cout << "Creating Shader " << prg << std::endl; }
	~Shader();

	const void bind() const;

	const void setUniform1i(const std::string& uniform_name, int v) const;

	const void setUniform1f(const std::string& uniform_name, float v) const;

	const void setUniform2i(const std::string& uniform_name, int v1, int v2) const;
	const void setUniform2i(const std::string& uniform_name, glm::i32vec2& value) const;

	const void setUniform2f(const std::string& uniform_name, float v1, float v2) const;
	const void setUniform2f(const std::string& uniform_name, glm::vec2& value) const;
	
	const void setUniform3i(const std::string& uniform_name, int v1, int v2, int v3) const;
	const void setUniform3i(const std::string& uniform_name, glm::i32vec3& value) const;

	const void setUniform3f(const std::string& uniform_name, float v1, float v2, float v3) const;
	const void setUniform3f(const std::string& uniform_name, glm::vec3& value) const;

	const void setUniform4i(const std::string& uniform_name, int v1, int v2, int v3, int v4) const;
	const void setUniform4i(const std::string& uniform_name, glm::i32vec4& value) const;

	const void setUniform4f(const std::string& uniform_name, float v1, float v2, float v3, float v4) const;
	const void setUniform4f(const std::string& uniform_name, glm::vec4& value) const;

	const void setUniformMatrix3f(const std::string& uniform_name, float value[9]) const;
	const void setUniformMatrix3f(const std::string& uniform_name, glm::mat3& value) const;

	const void setUniformMatrix4f(const std::string& uniform_name, float value[16]) const;
	const void setUniformMatrix4f(const std::string& uniform_name, glm::mat4& value) const;

	const void setUniformBlock(const std::string& block_name, unsigned int Slot) const;

private:
	const unsigned int getUniformLocation(const std::string& uniform_name) const;
	const unsigned int getUniformBlockLocation(const std::string& block_name) const;
};


