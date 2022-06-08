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

class SpecularMaterial {
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
private:

	unsigned int			 m_vao = 0;
	int						 m_size;
	GLbuffer				 m_uniform_buffer{ NULL, sizeof(UniformData) };
	std::shared_ptr<Texture> m_diff_texture = NULL;
	std::shared_ptr<Texture> m_spec_texture = NULL;
	std::shared_ptr<Texture> m_glos_texture = NULL;

public:
	SpecularMaterial() { }
	~SpecularMaterial();

	SpecularMaterial(const SpecularMaterial&) = delete;
	SpecularMaterial& operator=(const SpecularMaterial&) = delete;

	SpecularMaterial(SpecularMaterial&& other);
	SpecularMaterial& operator=(SpecularMaterial&& other);


	template<diff_map diff_t, spec_map spec_t, glos_map glos_t>
	SpecularMaterial(diff_t diff, spec_t spec, glos_t glos) : m_uniform_buffer(NULL, sizeof(UniformData))
	{
		set_diff(diff);
		set_spec(spec);
		set_glos(glos);
	}
	
	template<diff_map diff_t>
	void set_diff(diff_t diff) {
		if constexpr (std::is_same_v<diff_t, std::shared_ptr<Texture>>)
			m_diff_texture = diff;
		else
			m_uniform_buffer.setData(diff, offsetof(UniformData, diff));

		m_uniform_buffer.setData(std::is_same_v<diff_t, std::shared_ptr<Texture>>, offsetof(UniformData, hasD));
	}

	template<spec_map spec_t>
	void set_spec(spec_t spec) {
		if constexpr (std::is_same_v<spec_t, std::shared_ptr<Texture>>)
			m_spec_texture = spec;
		else
			m_uniform_buffer.setData(spec, offsetof(UniformData, spec));

		m_uniform_buffer.setData(std::is_same_v<spec_t, std::shared_ptr<Texture>>, offsetof(UniformData, hasS));
	}

	template<glos_map glos_t>
	void set_glos(glos_t glos) {
		if constexpr (std::is_same_v<glos_t, std::shared_ptr<Texture>>)
			m_glos_texture = glos;
		else
			m_uniform_buffer.setData(glos, offsetof(UniformData, glos));

		m_uniform_buffer.setData(std::is_same_v<glos_t, std::shared_ptr<Texture>>, offsetof(UniformData, hasG));
	}
	
	void bind();
};