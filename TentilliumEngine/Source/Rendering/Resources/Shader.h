#pragma once
#include "Resource.h"
#include <string>


// this shouldnt be public it should be completely internal
enum ShaderType : int {
	FRAG = 0x8b31,
	GEOM = 0x8dd9,
	VERT = 0x8b30,
	COMP = 0x91b9,
};

template<ShaderType type>
class Shader : public Resource<Shader<type>> {
	friend class Resource<Shader<type>>;
public:
	__declspec(property(get = get_handle)) unsigned int handle;

	Shader(std::string filepath);
	~Shader();

	Shader(const Shader&);
	Shader& operator=(const Shader&);

	unsigned int get_handle() { return m_handle; }

protected:
	unsigned int m_handle;
private:
	Shader(unsigned int handle) : m_handle(handle) { }

};

using FragmentShader = Shader<FRAG>;
using VertexShader = Shader<VERT>;
using ComputeShader = Shader<COMP>;
using GeometryShader = Shader<GEOM>;