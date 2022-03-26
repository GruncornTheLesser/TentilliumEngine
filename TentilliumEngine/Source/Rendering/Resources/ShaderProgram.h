#pragma once
#include "Shader.h"
#include "Resource.h"

#define MATERIAL_MAP_DIFF_NAME "_diffuse_map_"
#define MATERIAL_MAP_NORM_NAME "_normal_map_"
#define MATERIAL_MAP_SPEC_NAME "_specular_map_"
#define MATERIAL_MAP_SHIN_NAME "_shininess_map_"
#define MATERIAL_MAP_DIFF_SLOT 0
#define MATERIAL_MAP_NORM_SLOT 1
#define MATERIAL_MAP_SPEC_SLOT 2
#define MATERIAL_MAP_SHIN_SLOT 3

class ShaderProgram final : public Resource<ShaderProgram> {
private:
	struct ShaderLink { // stores the shared pointers to keep the resource from being destroyed
		std::shared_ptr<VertexShader> vert = nullptr;
		std::shared_ptr<GeometryShader> geom = nullptr;
		std::shared_ptr<FragmentShader> frag = nullptr;
		std::shared_ptr<ComputeShader> comp = nullptr;

		bool valid() { return (vert && geom && frag) || (vert && frag) || (comp); }
	} m_shaders;

	unsigned int m_program;
	mutable std::unordered_map<std::string, unsigned int> uniform_cache;	// uniform locations
	mutable std::unordered_map<std::string, unsigned int> block_cache;		// uniform block locations

private:
	unsigned int createProgram(ShaderLink shaders);
public:
	ShaderProgram(std::shared_ptr<VertexShader> vertex, std::shared_ptr<GeometryShader> geometry, std::shared_ptr<FragmentShader> fragment)
		: m_program(createProgram(ShaderLink{ vertex, geometry, fragment, nullptr })) { }
	ShaderProgram(std::shared_ptr<VertexShader> vertex, std::shared_ptr<FragmentShader> fragment)
		: m_program(createProgram(ShaderLink{ vertex, nullptr, fragment, nullptr })) { }
	ShaderProgram(std::shared_ptr<ComputeShader> compute) 
		: m_program(createProgram(ShaderLink{ nullptr, nullptr, nullptr, compute })) { }
	ShaderProgram(std::string filepath);
	~ShaderProgram();

	bool hasUniform(const std::string& uniform_name);
	bool hasUniformBlock(const std::string& uniform_name);

	void bind() const;

	void setUniform1i(const std::string& uniform_name, int v) const;
	void setUniform1f(const std::string& uniform_name, float v) const;
	void setUniform2i(const std::string& uniform_name, glm::ivec2& value) const;
	void setUniform2f(const std::string& uniform_name, glm::vec2& value) const;
	void setUniform3i(const std::string& uniform_name, glm::ivec3& value) const;
	void setUniform3f(const std::string& uniform_name, glm::vec3& value) const;
	void setUniform4i(const std::string& uniform_name, glm::ivec4& value) const;
	void setUniform4f(const std::string& uniform_name, glm::vec4& value) const;
	void setUniformMatrix3f(const std::string& uniform_name, glm::mat3& value) const;
	void setUniformMatrix4f(const std::string& uniform_name, glm::mat4& value) const;
	void setUniformBlock(const std::string& block_name, unsigned int slot) const;

private:
	unsigned int getUniformLocation(const std::string& uniform_name) const;
	unsigned int getUniformBlockLocation(const std::string& block_name) const;
};
