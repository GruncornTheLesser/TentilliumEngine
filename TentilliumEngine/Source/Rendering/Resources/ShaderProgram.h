#pragma once
#include "Resource.h"
#include "Shader.h"
#include "Texture.h"

#include <glm.hpp>
#include <unordered_map>

class ShaderProgram final {
private:
	unsigned int m_program;
	Shader<ShaderType::COMP> m_compute{0};
	Shader<ShaderType::VERT> m_vertex{0};
	Shader<ShaderType::GEOM> m_geometry{0};
	Shader<ShaderType::FRAG> m_fragment{0};
	
	mutable std::unordered_map<std::string, unsigned int> m_uniform_cache;	// uniform locations
	mutable std::unordered_map<std::string, unsigned int> m_block_cache;	// uniform block locations

	unsigned int createProgram(unsigned int compute, unsigned int vertex, unsigned int geometry, unsigned int fragment);

public:
	ShaderProgram(std::string vertex, std::string geometry, std::string fragment, std::string compute);

	ShaderProgram(std::string filepath);
	~ShaderProgram();

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	ShaderProgram(const ShaderProgram&&);
	ShaderProgram& operator=(const ShaderProgram&&);

	void bind() const;

	void setUniform1i(std::string uniform_name, int value) const;
	void setUniform1f(std::string uniform_name, float value) const;
	void setUniform2i(std::string uniform_name, const glm::ivec2& value) const;
	void setUniform2f(std::string uniform_name, const glm::vec2& value) const;
	void setUniform3i(std::string uniform_name, const glm::ivec3& value) const;
	void setUniform3f(std::string uniform_name, const glm::vec3& value) const;
	void setUniform4i(std::string uniform_name, const glm::ivec4& value) const;
	void setUniform4f(std::string uniform_name, const glm::vec4& value) const;
	void setUniformMat2(std::string uniform_name, const glm::mat2& value) const;
	void setUniformMat3(std::string uniform_name, const glm::mat3& value) const;
	void setUniformMat4(std::string uniform_name, const glm::mat4& value) const;

	void setUniformBlock(std::string, unsigned int value) const;

private:
	unsigned int getUniformLocation(std::string uniform_name) const;
	unsigned int getUniformBlockLocation(std::string block_name) const;
};
