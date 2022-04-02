#pragma once
#include "Shader.h"
#include "Resource.h"
#include <glm.hpp>
#include <unordered_map>

class ShaderProgram final : public Resource<ShaderProgram> {
private:
	struct ShaderLink { // stores the shared pointers to keep the resource from being destroyed
		std::shared_ptr<VertexShader>	vert = nullptr;
		std::shared_ptr<GeometryShader> geom = nullptr;
		std::shared_ptr<FragmentShader> frag = nullptr;
		std::shared_ptr<ComputeShader>	comp = nullptr;
		//std::shared_ptr<TesselationShader> frag = nullptr;
		//std::shared_ptr<TesselationShader2> frag = nullptr;
	} m_shaders;

	unsigned int m_program;

	mutable std::unordered_map<std::string, unsigned int> m_uniform_cache;	// uniform locations
	mutable std::unordered_map<std::string, unsigned int> m_block_cache;		// uniform block locations

	unsigned int createProgram(ShaderLink shaders);

public:
	ShaderProgram(std::shared_ptr<VertexShader> vertex, std::shared_ptr<GeometryShader> geometry, std::shared_ptr<FragmentShader> fragment)
		: m_program(createProgram(ShaderLink{ vertex, geometry, fragment })) { }
	ShaderProgram(std::shared_ptr<VertexShader> vertex, std::shared_ptr<FragmentShader> fragment)
		: m_program(createProgram(ShaderLink{ vertex, nullptr, fragment })) { }

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
