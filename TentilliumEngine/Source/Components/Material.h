#pragma once
#include "../Rendering/Resources/Texture.h"
#include "../Rendering/Resources/GLbuffer.h"
#include <type_traits>
#include <unordered_map>
#include <glm.hpp>
#include <variant>

template <class T, class ... MapTypes>
concept Map = std::is_same_v<T, Texture> ||
			  (std::is_same_v<T, MapTypes> || ...);

class Material {
	friend class RenderSystem;

public:
	template<Map<glm::vec4, glm::vec3> diffuse_t, Map<float> specular_t, Map<float> gloss_t, Map<glm::vec3> normal_t>
	Material(const diffuse_t& diffuse, const specular_t& specular, const gloss_t& gloss, const normal_t& normal)
		: Material()
	{
		setDiffuse(diffuse);
		setSpecular(specular);
		setGloss(gloss);
		setNormal(normal);
	}

	template<Map<glm::vec4, glm::vec3> diffuse_t, Map<float> specular_t, Map<float> gloss_t>
	Material(const diffuse_t& diffuse, const specular_t& specular, const gloss_t& gloss)
		: Material()
	{
		setDiffuse(diffuse);
		setSpecular(specular);
		setGloss(gloss);
	}

	template<Map<glm::vec4, glm::vec3> diffuse_t, Map<float> specular_t>
	Material(const diffuse_t& diffuse, const specular_t& specular)
		: Material()
	{
		setDiffuse(diffuse);
		setSpecular(specular);
	}

	template<Map<glm::vec4, glm::vec3> diffuse_t>
	Material(const diffuse_t& diffuse) : Material()
	{
		setDiffuse(diffuse);
	}

	Material() : m_uniformBuffer(UniformData{ glm::vec4(1, 1, 1, 1), 1, 1, false, false, false, false }) { }

	void setDiffuse(const glm::vec4& value);
	void setDiffuse(const glm::vec3& value);
	void setDiffuse(const Texture& value);

	void setSpecular(float value);
	void setSpecular(const Texture& value);

	void setGloss(float value);
	void setGloss(const Texture& value);

	void setNormal(const Texture& value);

	void bind(int uniformBufferIndex) const;

private:
	struct UniformData {
		glm::vec4 diffuse;
		float specular;
		float gloss;
		bool hasDiffuseMap;
		bool hasSpecularMap;
		bool hasGlossMap;
		bool hasNormalMap;
	};

	GLbuffer m_uniformBuffer;

	Texture m_DiffuseMap{ 0 }; // initiated to 0
	Texture m_specularMap{ 0 };
	Texture m_glossMap{ 0 };
	Texture m_normalMap{ 0 };
};

/*
namespace Mat {
	// [diffuse, specular, glossiness]
	// [diffuse, metallic, roughness]

	template <class T1, class T2>
	concept MapType = std::is_same_v<T1, const Texture&> || std::is_same_v<T1, T2>;

	template<typename T>
	class Map {
	public:
		template<MapType<T> value_t>
		Map(const value_t& value) { m_value = value; }
		void bind(unsigned int& slot) const { 
			if (auto ptr = std::get_if<Texture>(m_value)) 
				ptr->bind(slot);
		}

	protected:
		std::variant<Texture, T> m_value;
	};

	class DiffuseMap : Map<glm::vec4> { };
	class MetallicMap : Map<float> {};
	class RoughnessMap : Map<float> {};
	class AOMap : Map<float>{};
	class NormalMap : Texture {};

	class SpecularMaterial : GLbuffer { };
	class PBRMaterial : GLbuffer { };

	//TAG(TransparentTag);
}
*/