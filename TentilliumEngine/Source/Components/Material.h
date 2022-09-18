#pragma once
#include "../Rendering/Resources/Texture.h"
#include "../Rendering/Resources/GLbuffer.h"
#include <type_traits>
#include <unordered_map>
#include <glm.hpp>


template <typename T>
concept Map = std::is_same_v<T, Texture> || std::is_same_v<T, glm::vec4>;

class Material final : protected GLbuffer {
	friend class RenderSystem;
private:
	struct UniformData {
		glm::vec4		diffuse;
		glm::vec4		specular;
		bool			hasDiffuseMap;
		bool			hasSpecularMap;
		bool			hasNormalMap;
	};

	std::unordered_map<unsigned int, Texture> m_textures;

public:
	Material() : GLbuffer(nullptr, sizeof(UniformData))
	{ }
	
	Material(Texture diff)
		: GLbuffer(nullptr, sizeof(UniformData))
	{
		set_diffuse(diff);
	}

	Material(glm::vec4 diff)
		: GLbuffer(nullptr, sizeof(UniformData))
	{
		set_diffuse(diff);
	}

	void set_diffuse(Texture texture);
	void set_diffuse(glm::vec4 value);

	void set_specular(Texture texture);
	void set_specular(glm::vec4 value);

	void set_normal(Texture texture);

	void bind(int index = 0, int texture_slot_offset = 0) const;
};