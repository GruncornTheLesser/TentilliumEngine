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
		int hasDiffuseMap;
		int hasSpecularMap;
		int hasGlossMap;
		int hasNormalMap;
	};

	Buffer m_uniformBuffer;

	std::optional<Texture> m_diffuseMap;	
	std::optional<Texture> m_specularMap;
	std::optional<Texture> m_glossMap;
	std::optional<Texture> m_normalMap;
};

class Mat2 {
public:
	Mat2(
		std::variant<std::string, glm::vec4> diffuse = glm::vec4(1, 1, 1, 1),
		std::variant<std::string, float> specular = 1.0f,
		std::variant<std::string, float> gloss = 1.0f,
		std::optional<std::string> normal = std::optional<std::string>{});

private:
	Buffer m_uniformBuffer;

	unsigned int m_textureArray;
	// assimp material attributes
	// string name
	// vec3 color_diffuse
	// vec3 color_specular
	// vec3 color_ambient
	// vec3 color_emissive
	// vec3 color_transparent
	// 
	// bool use_wire_frame
	// bool use_back_face_culling
	// enum shading_model
	// enum blend_func
	// 
	// float opacity
	// float shininess
	// float shininess_strength
	// 
	// float refractiveIndex
	// 
	// 
	// 
	// 
	// 
	// diffuse		or base colour
	// specular		or metallic
	// gloss		or roughness
	// normal 
	// ao
	// bump
	// emissive
	// 
	// Material Texture Packing
	



};