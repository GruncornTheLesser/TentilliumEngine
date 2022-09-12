#pragma once
#include "Resource.h"
#include <string>


// this shouldnt be public it should be completely internal
enum class ShaderType : int {
	FRAG = 0x8b31,
	GEOM = 0x8dd9,
	VERT = 0x8b30,
	COMP = 0x91b9,
};

template<ShaderType type>
class Shader : public Resource<Shader<type>> {
	friend class ShaderProgram;
	friend class Resource<Shader<type>>;
	using Resource<Shader<type>>::create;
	using Resource<Shader<type>>::destroy;

private:
	Shader(unsigned int handle) : m_handle(handle) { }
public:
	Shader() : m_handle(0) { }
	Shader(std::string filepath);
	~Shader();

	Shader(const Shader&);
	Shader& operator=(const Shader&);

protected:
	unsigned int m_handle;
};

using FragmentShader = Shader<ShaderType::FRAG>;
using GeometryShader = Shader<ShaderType::GEOM>;
using VertexShader = Shader<ShaderType::VERT>;
using ComputeShader = Shader<ShaderType::COMP>;