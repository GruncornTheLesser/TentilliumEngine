#pragma once
#include "../Rendering/Resources/Texture.h"
#include "../Rendering/Resources/Buffer.h"
#include <glm.hpp>
#include <variant>
#include <optional>

class Material {
	friend class RenderSystem;

public:
	Material(
		std::variant<Texture, glm::vec4> diffuse = glm::vec4(1, 1, 1, 1),
		std::variant<Texture, float> specular = 1.0f,
		std::variant<Texture, float> gloss = 1.0f,
		std::optional<Texture> normal = std::optional<Texture>{});

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

	std::optional<Texture> m_diffuseMap;
	std::optional<Texture> m_specularMap;
	std::optional<Texture> m_glossMap;
	std::optional<Texture> m_normalMap;
};