#pragma once
#include <GLM/glm.hpp>		// mathematics
#include <unordered_map>
#include <string>

class Shader
{
private:
	unsigned int m_program;
	mutable std::unordered_map<std::string, unsigned int> uniform_cache; // uniform locations
	mutable std::unordered_map<std::string, unsigned int> block_cache; // uniform block locations
public:
	Shader(const char* filepath);
	~Shader();

	void Bind() const;
	void UnBind() const;

	void SetUniform1i(const std::string& uniform_name, int v) const;

	void SetUniform1f(const std::string& uniform_name, float v) const;

	void SetUniform2i(const std::string& uniform_name, int v1, int v2) const;
	void SetUniform2i(const std::string& uniform_name, glm::i32vec2& value) const;

	void SetUniform2f(const std::string& uniform_name, float v1, float v2) const;
	void SetUniform2f(const std::string& uniform_name, glm::vec2& value) const;
	
	void SetUniform3i(const std::string& uniform_name, int v1, int v2, int v3) const;
	void SetUniform3i(const std::string& uniform_name, glm::i32vec3& value) const;

	void SetUniform3f(const std::string& uniform_name, float v1, float v2, float v3) const;
	void SetUniform3f(const std::string& uniform_name, glm::vec3& value) const;

	void SetUniform4i(const std::string& uniform_name, int v1, int v2, int v3, int v4) const;
	void SetUniform4i(const std::string& uniform_name, glm::i32vec4& value) const;

	void SetUniform4f(const std::string& uniform_name, float v1, float v2, float v3, float v4) const;
	void SetUniform4f(const std::string& uniform_name, glm::vec4& value) const;

	void SetUniformMatrix3f(const std::string& uniform_name, float value[9]) const;
	void SetUniformMatrix3f(const std::string& uniform_name, glm::mat3& value) const;

	void SetUniformMatrix4f(const std::string& uniform_name, float value[16]) const;
	void SetUniformMatrix4f(const std::string& uniform_name, glm::mat4& value) const;

	void SetUniformBlock(const std::string& block_name, unsigned int Slot) const;

private:
	unsigned int GetUniformLocation(const std::string& uniform_name) const;
	unsigned int GetUniformBlockLocation(const std::string& block_name) const;

};

