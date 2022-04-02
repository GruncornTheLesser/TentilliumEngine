#pragma once
#include "Resource.h"
#include <string>

enum class ShaderType : int { FRAG, GEOM, VERT, COMP, TCON, TEVA };

namespace internal {
	enum class loadType { FROM_FILE, FROM_TEXT };

	class Shader {
	public:
		unsigned int m_shader;
		Shader(ShaderType type, std::string text);
		~Shader();

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		Shader(const Shader&&);
		Shader& operator=(const Shader&&);
	};
}

template<ShaderType type>
class Shader final : private internal::Shader, public Resource<Shader<type>>
{
	friend class ShaderProgram;
public:
	Shader(std::string text) : internal::Shader(type, text) { }
};

using FragmentShader =	Shader<ShaderType::FRAG>;
using GeometryShader =	Shader<ShaderType::GEOM>;
using VertexShader =	Shader<ShaderType::VERT>;
using ComputeShader =	Shader<ShaderType::COMP>;
using TessContShader =	Shader<ShaderType::TCON>;
using TessEvalShader =	Shader<ShaderType::TEVA>;


