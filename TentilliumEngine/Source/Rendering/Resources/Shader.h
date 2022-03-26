#pragma once
#include "Resource.h"
#include <unordered_map>
#include <string>
#include <glm.hpp>
#include <iostream>

enum class ShaderType : int { FRAG, GEOM, VERT, COMP };


namespace internal {
	enum class loadType { FROM_FILE, FROM_TEXT };


	class Shader {
	public:
		unsigned int m_shader;
		Shader(ShaderType type, std::string text, loadType method = loadType::FROM_FILE);
		~Shader();
	};


}

template<ShaderType type>
class Shader final : private internal::Shader, public Resource<Shader<type>>
{
	friend class ShaderProgram;
public:
	Shader(std::string text, internal::loadType method = internal::loadType::FROM_FILE) : internal::Shader(type, text, method) { }
};

using FragmentShader = Shader<ShaderType::FRAG>;
using GeometryShader = Shader<ShaderType::GEOM>;
using VertexShader = Shader<ShaderType::VERT>;
using ComputeShader = Shader<ShaderType::COMP>;


