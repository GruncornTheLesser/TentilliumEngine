#include "ShaderProgram.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths
#include <gtc/type_ptr.hpp>
#include <filesystem>

unsigned int ShaderProgram::createProgram(ShaderLink shaders) {

	m_program = glCreateProgram();

	// if shader in program, attach it
	if (shaders.vert.get()) glAttachShader(m_program, shaders.vert->m_shader);
	if (shaders.geom.get()) glAttachShader(m_program, shaders.geom->m_shader);
	if (shaders.frag.get()) glAttachShader(m_program, shaders.frag->m_shader);
	if (shaders.comp.get()) glAttachShader(m_program, shaders.comp->m_shader);
	
	int infoLen;
	int status;

	glLinkProgram(m_program);

	glGetProgramiv(m_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLen);
		char* message = (char*)alloca(infoLen * sizeof(char));
		glGetProgramInfoLog(m_program, infoLen, NULL, message);

		std::cerr << "Failed to link program.\nErrorLog: '" << message << "'" << std::endl;
		throw std::exception();
	}

	glValidateProgram(m_program);

	glGetProgramiv(m_program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLen);
		char* message = (char*)alloca(infoLen * sizeof(char));
		glGetProgramInfoLog(m_program, infoLen, NULL, message);

		std::cerr << "Failed to validate program.\nErrorLog: '" << message << "'" << std::endl;
		throw std::exception();
	}

	return m_program;
}

ShaderProgram::ShaderProgram(std::string filepath)
{
	std::string fp(filepath);

	size_t p1 = fp.find_last_of("\\/");
	size_t p2 = fp.find_last_of(".");

	std::string dirPath = fp.substr(0, p1 == std::string::npos ? 0 : p1);
	std::string dirName = fp.substr(p1 == std::string::npos ? 0 : p1 + 1, (p2 == std::string::npos ? fp.length() : (p2 - 1)) - p1);

	ShaderLink shaders;
	for (const auto& file : std::filesystem::directory_iterator(dirPath)) {
		std::string fp = file.path().string();

		size_t p1 = fp.find_last_of("\\/");
		size_t p2 = fp.find_last_of(".");

		if (p1 == std::string::npos) p1 = 0; else p1 += 1;
		if (p2 == std::string::npos) p2 = fp.length();

		std::string fileName = fp.substr(p1, p2 - p1);
		std::string fileExt = fp.substr(p2, fp.length() - p2);

		if (dirName == fileName) {
			if		(fileExt == ".frag") 
				shaders.frag = Shader<ShaderType::FRAG>::load(file.path().string().c_str());
			else if (fileExt == ".geom") 
				shaders.geom = Shader<ShaderType::GEOM>::load(file.path().string().c_str());
			else if (fileExt == ".vert") 
				shaders.vert = Shader<ShaderType::VERT>::load(file.path().string().c_str());
			else if (fileExt == ".comp")
				shaders.comp = Shader<ShaderType::COMP>::load(file.path().string().c_str());
		}
	}

	createProgram(shaders);
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_program);
}

ShaderProgram::ShaderProgram(const ShaderProgram&& other) {
	this->m_program = other.m_program;
}

ShaderProgram& ShaderProgram::operator=(const ShaderProgram&& other) {
	glDeleteProgram(this->m_program);
	this->m_program = other.m_program;
	this->m_uniform_cache.clear();
	return *this;
}

void ShaderProgram::bind() const 
{
	glUseProgram(m_program);
}

void ShaderProgram::setUniform1i(std::string uniform_name, int v) const
{
	glProgramUniform1i(m_program, getUniformLocation(uniform_name), v);
}

void ShaderProgram::setUniform1f(std::string uniform_name, float v) const
{
	glProgramUniform1f(m_program, getUniformLocation(uniform_name), v);
}

void ShaderProgram::setUniform2i(std::string uniform_name, const glm::ivec2& value) const
{
	glProgramUniform2iv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform2f(std::string uniform_name, const glm::vec2& value) const
{
	glProgramUniform2fv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform3i(std::string uniform_name, const glm::ivec3& value) const
{
	glProgramUniform3iv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform3f(std::string uniform_name, const glm::vec3& value) const
{
	glProgramUniform3fv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform4i(std::string uniform_name, const glm::ivec4& value) const
{
	glProgramUniform4iv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform4f(std::string uniform_name, const glm::vec4& value) const
{
	glProgramUniform4fv(m_program, getUniformLocation(uniform_name), 1, glm::value_ptr(value));
}



void ShaderProgram::setUniformMat2(std::string uniform_name, const glm::mat2& value) const
{
	glProgramUniformMatrix2fv(m_program, getUniformLocation(uniform_name), 1, false, glm::value_ptr(value));
}

void ShaderProgram::setUniformMat3(std::string uniform_name, const glm::mat3& value) const
{
	glProgramUniformMatrix3fv(m_program, getUniformLocation(uniform_name), 1, false, glm::value_ptr(value));
}

void ShaderProgram::setUniformMat4(std::string uniform_name, const glm::mat4& value) const
{
	glProgramUniformMatrix4fv(m_program, getUniformLocation(uniform_name), 1, false, glm::value_ptr(value));
}

void ShaderProgram::setUniformBlock(std::string block_name, unsigned int Binding) const
{
	glUniformBlockBinding(m_program, getUniformBlockLocation(block_name), Binding);
}

unsigned int ShaderProgram::getUniformLocation(std::string uniform_name) const
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

unsigned int ShaderProgram::getUniformBlockLocation(std::string block_name) const
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

