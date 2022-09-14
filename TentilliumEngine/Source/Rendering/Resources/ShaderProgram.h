#pragma once
#include "Resource.h"
#include "Shader.h"
#include <glm.hpp>
#include <unordered_map>

template<ShaderType ... Ts>
concept is_ComputeProgram = (... & Ts) == COMP;

template<ShaderType ... Ts>
class ShaderProgram : Shader<Ts>... {
public:
	ShaderProgram(std::string filepath);
	ShaderProgram(Shader<Ts> ... shaders);

	~ShaderProgram();

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	ShaderProgram(ShaderProgram&&);
	ShaderProgram& operator=(ShaderProgram&&);

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

	void dispatch(glm::uvec3 workGroups) requires is_ComputeProgram<Ts...>;

private:
	unsigned int m_program;
	mutable std::unordered_map<std::string, unsigned int> m_uniform_cache;	// uniform locations
	mutable std::unordered_map<std::string, unsigned int> m_block_cache;	// uniform block locations

	unsigned int genProgram();

	unsigned int getUniformLocation(std::string uniform_name) const;
	unsigned int getUniformBlockLocation(std::string block_name) const;
};

using ComputeProgram = ShaderProgram<COMP>;
using ShadingProgram = ShaderProgram<VERT, FRAG>;
