#pragma once
#include "../Rendering/Resources/Texture.h"
#include "../Rendering/Resources/Buffer.h"
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

	Buffer m_uniformBuffer;

	Texture m_DiffuseMap{ 0 }; // initiated to 0
	Texture m_specularMap{ 0 };
	Texture m_glossMap{ 0 };
	Texture m_normalMap{ 0 };
};
/*
namespace Material {
	class UBO : private GLbuffer {
		friend class MaterialSystem;

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
	};

	class DiffuseMap final {
		friend class MaterialSystem;
	public:
		DiffuseMap(const Texture& texture) { m_value = texture; }
		DiffuseMap(const glm::vec4& value) { m_value = value; }
	private:
		std::variant<Texture, glm::vec4> m_value;
	};

	class SpecularMap final {
		friend class MaterialSystem;
	public:
		SpecularMap(const Texture& texture) { m_value = texture; }
		SpecularMap(const glm::vec4& value) { m_value = value; }
	private:
		std::variant<Texture, glm::vec4> m_value;
	};

	class GlossMap final {
		friend class MaterialSystem;
	public:
		GlossMap(const Texture& texture) { m_value = texture; }
		GlossMap(const glm::vec4& value) { m_value = value; }
	private:
		std::variant<Texture, glm::vec4> m_value;
	};

	class NormalMap final {
	public:
		NormalMap(const Texture& texture) { m_value = texture; }
		NormalMap(const glm::vec4& value) { m_value = value; }
	private:
		std::variant<Texture, glm::vec4> m_value;
	};

	TAG(UpdateTag);
}
*/