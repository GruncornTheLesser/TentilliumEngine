#pragma once
#include "../Rendering/Resources/Texture.h"
#include "../Rendering/Resources/GLbuffer.h"
#include <type_traits>
#include <glm.hpp>


template <typename T>
concept diff_map = std::is_same_v<T, std::shared_ptr<Texture>> || std::is_same_v<T, glm::vec3>;

template <typename T>
concept spec_map = std::is_same_v<T, std::shared_ptr<Texture>> || std::is_same_v<T, float>;

template <typename T>
concept glos_map = std::is_same_v<T, std::shared_ptr<Texture>> || std::is_same_v<T, float>;

class SpecularMaterial final : protected GLbuffer {
	friend class SpecularSystem;
private:
	struct UniformData {
		glm::vec3		diff;
		unsigned int	hasD;
		float			spec;
		unsigned int	hasS;
		float			glos;
		unsigned int	hasG;
	};

	std::map<unsigned int, std::shared_ptr<Texture>> m_textures{};

public:
	SpecularMaterial() : GLbuffer(nullptr, sizeof(UniformData)) { }

	template<diff_map diff_t, spec_map spec_t, glos_map glos_t>
	SpecularMaterial(diff_t diff, spec_t spec, glos_t glos)
		: GLbuffer(nullptr, sizeof(UniformData))
	{
		set_diff(diff);
		set_spec(spec);
		set_glos(glos);
	}
	
	void set_diff(std::shared_ptr<Texture> texture) {
		m_textures[0] = texture;
		set_data(true, offsetof(UniformData, hasD));
	}
	void set_diff(glm::vec3 value) {
		m_textures.erase(0);
		set_data(value, offsetof(UniformData, diff));
		set_data(false, offsetof(UniformData, hasD));
	}

	void set_spec(std::shared_ptr<Texture> texture) {
		m_textures[1] = texture;
		set_data(false, offsetof(UniformData, hasS));
	}
	void set_spec(float value) {
		m_textures.erase(1);
		set_data(value, offsetof(UniformData, spec));
		set_data(false, offsetof(UniformData, hasS));
	}
	
	void set_glos(std::shared_ptr<Texture> texture) {
		m_textures[2] = texture;
		set_data(false, offsetof(UniformData, hasG));
	}
	void set_glos(float value) {
		m_textures.erase(2);
		set_data(value, offsetof(UniformData, glos));
		set_data(false, offsetof(UniformData, hasG));
	}

	void bind() {
		GLbuffer::bind_uniform_slot(0);

		for (auto [slot, texture] : m_textures)
			texture->bindSlot(slot);
	}
};