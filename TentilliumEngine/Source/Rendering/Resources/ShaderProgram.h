#pragma once
#include "Resource.h"
#include "Shader.h"
#include <glm.hpp>
#include <unordered_map>

template<ShaderType ... Ts>
concept IsComputeProgram = sizeof...(Ts) == 1 && ((Ts == COMP) & ...);

template<ShaderType ... Ts>
class ShaderProgram : Shader<Ts>..., Shared<ShaderProgram<Ts...>> {
	friend class Shared<ShaderProgram<Ts...>>;
public:
	ShaderProgram(std::string filepath);

	void bind() const;

	void setUniform(int location, int value) const;
	void setUniform(int location, float value) const;
	void setUniform(int location, const glm::ivec2& value) const;
	void setUniform(int location, const glm::vec2& value) const;
	void setUniform(int location, const glm::ivec3& value) const;
	void setUniform(int location, const glm::vec3& value) const;
	void setUniform(int location, const glm::ivec4& value) const;
	void setUniform(int location, const glm::vec4& value) const;
	void setUniform(int location, const glm::mat2& value) const;
	void setUniform(int location, const glm::mat3& value) const;
	void setUniform(int location, const glm::mat4& value) const;

	void setUniform(std::string uniform_name, int value) const;
	void setUniform(std::string uniform_name, float value) const;
	void setUniform(std::string uniform_name, const glm::ivec2& value) const;
	void setUniform(std::string uniform_name, const glm::vec2& value) const;
	void setUniform(std::string uniform_name, const glm::ivec3& value) const;
	void setUniform(std::string uniform_name, const glm::vec3& value) const;
	void setUniform(std::string uniform_name, const glm::ivec4& value) const;
	void setUniform(std::string uniform_name, const glm::vec4& value) const;
	void setUniform(std::string uniform_name, const glm::mat2& value) const;
	void setUniform(std::string uniform_name, const glm::mat3& value) const;
	void setUniform(std::string uniform_name, const glm::mat4& value) const;

	void setUniformBlock(int location, unsigned int value) const;
	void setUniformBlock(std::string name, unsigned int value) const;

	void dispatch(glm::uvec3 workGroups) requires IsComputeProgram<Ts...>;

	unsigned int getLocation(std::string uniform_name) const;
	unsigned int getBlockLocation(std::string block_name) const;

private:
	static void destroy(unsigned int handle);
};
