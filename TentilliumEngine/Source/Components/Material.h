#pragma once
#include "../Rendering/Resources/Texture.h"
#include "../Rendering/Resources/GLbuffer.h"
#include <type_traits>
#include <unordered_map>
#include <glm.hpp>


template <typename T>
concept diffuse = std::is_same_v<T, Texture> || std::is_same_v<T, glm::vec3>;

template <typename T>
concept specular = std::is_same_v<T, Texture> || std::is_same_v<T, float>;

template <typename T>
concept gloss = std::is_same_v<T, Texture> || std::is_same_v<T, float>;

class Material final : protected GLbuffer {
	friend class MaterialSystem;
private:
	struct UniformData {
		glm::vec3		diff;
		unsigned int	hasD;
		float			spec;
		unsigned int	hasS;
		float			glos;
		unsigned int	hasG;
	};


	std::unordered_map<unsigned int, Texture> m_textures;

public:
	Material() : GLbuffer(nullptr, sizeof(UniformData))
	{ }
	template<diffuse diff_t, specular spec_t, gloss glos_t>
	Material(diff_t diff, spec_t spec, glos_t glos)
		: GLbuffer(nullptr, sizeof(UniformData))
	{
		set_diff(diff);
		set_spec(spec);
		set_glos(glos);
	}

	void set_diff(Texture texture);
	void set_diff(glm::vec3 value);

	void set_spec(Texture texture);
	void set_spec(float value);

	void set_glos(Texture texture);
	void set_glos(float value);

	void bind();
};