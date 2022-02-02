#pragma once
#include "Resource.h"
#include <unordered_map>
#include <string>
#include <glm.hpp>
using namespace glm;


struct Shader : Resource<Shader>
{
private:
	unsigned int m_program;
	mutable std::unordered_map<std::string, unsigned int> uniform_cache;	// uniform locations
	mutable std::unordered_map<std::string, unsigned int> block_cache;		// uniform block locations

	
public:
	Shader(unsigned int prg) : m_program(prg) { }
	~Shader();

	void bind() const;

	void setUniform1i(const std::string& uniform_name, int v) const;

	void setUniform1f(const std::string& uniform_name, float v) const;

	void setUniform2i(const std::string& uniform_name, int v1, int v2) const;
	void setUniform2i(const std::string& uniform_name, i32vec2& value) const;

	void setUniform2f(const std::string& uniform_name, float v1, float v2) const;
	void setUniform2f(const std::string& uniform_name, vec2& value) const;
	
	void setUniform3i(const std::string& uniform_name, int v1, int v2, int v3) const;
	void setUniform3i(const std::string& uniform_name, i32vec3& value) const;

	void setUniform3f(const std::string& uniform_name, float v1, float v2, float v3) const;
	void setUniform3f(const std::string& uniform_name, vec3& value) const;

	void setUniform4i(const std::string& uniform_name, int v1, int v2, int v3, int v4) const;
	void setUniform4i(const std::string& uniform_name, i32vec4& value) const;

	void setUniform4f(const std::string& uniform_name, float v1, float v2, float v3, float v4) const;
	void setUniform4f(const std::string& uniform_name, vec4& value) const;

	void setUniformMatrix3f(const std::string& uniform_name, float value[9]) const;
	void setUniformMatrix3f(const std::string& uniform_name, mat3& value) const;

	void setUniformMatrix4f(const std::string& uniform_name, float value[16]) const;
	void setUniformMatrix4f(const std::string& uniform_name, mat4& value) const;

	void setUniformBlock(const std::string& block_name, unsigned int Slot) const;

private:
	unsigned int getUniformLocation(const std::string& uniform_name) const;
	unsigned int getUniformBlockLocation(const std::string& block_name) const;
};

const Shader* Resource<Shader>::Load(const char* filepath);

