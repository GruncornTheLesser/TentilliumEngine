#pragma once
#include "../Rendering/Resources/Texture.h"
#include "../Rendering/Resources/Buffer.h"
#include <glm.hpp>
#include <variant>
#include <optional>

class Material {
public:
	struct InitData {
		std::variant<Texture, glm::vec3> diffuse = glm::vec3(1, 1, 1);
		std::variant<Texture, float> opacity = 1.0f;
		std::variant<Texture, glm::vec3> specular = glm::vec3(1, 1, 1);
		std::variant<Texture, float> shininess = 1.0f;
		std::variant<Texture, glm::vec3> emissive = glm::vec3(0, 0, 0);
		std::variant<Texture, float> ambientOcclusion = 1.0f;
		std::optional<Texture> normal = std::optional<Texture>{};
		std::optional<Texture> height = std::optional<Texture>{};
	};

	struct UniformData {
		glm::vec3 diffuse;
		float opacity;
		glm::vec3 specular;
		float shininess;
		glm::vec3 emissive;
		float ambientOcclusion;

		uint64_t diffuseMap;
		uint64_t opacityMap;
		uint64_t specularMap;
		uint64_t shininessMap;
		uint64_t emissiveMap;
		uint64_t ambientOcclusionMap;
		uint64_t normalMap;
		uint64_t heightMap;

		int diffuseHasMap;
		int opacityHasMap;
		int specularHasMap;
		int shininessHasMap;
		int emissiveHasMap;
		int ambientOcclusionHasMap;
		int normalHasMap;
		int heightHasMap;
	};

	Material(const InitData& data);
	Material(std::variant<Texture, glm::vec3> diffuse = glm::vec3(1, 1, 1),
			 std::variant<Texture, float> opacity = 1.0f,
			 std::variant<Texture, glm::vec3> specular = glm::vec3(1, 1, 1),
			 std::variant<Texture, float> shininess = 1.0f,
			 std::variant<Texture, glm::vec3> emissive = glm::vec3(0, 0, 0),
			 std::variant<Texture, float> ambientOcclusion = 1.0f,
			 std::optional<Texture> normal = std::optional<Texture>{},
			 std::optional<Texture> height = std::optional<Texture>{});

	void bind(int uniformBufferIndex) const; 

private:
	Buffer m_uniformBuffer;
	std::vector<Texture> m_maps;
};


class mat {
public:
	struct InitData {
		std::variant<Texture, glm::vec3> diffuse = glm::vec3(1, 1, 1);
		std::variant<Texture, float> opacity = 1.0f;
		std::variant<Texture, glm::vec3> specular = glm::vec3(1, 1, 1);
		std::variant<Texture, float> shininess = 1.0f;
		std::variant<Texture, glm::vec3> emissive = glm::vec3(0, 0, 0);
		std::variant<Texture, float> ambientOcclusion = 1.0f;
		std::optional<Texture> normal = std::optional<Texture>{};
		std::optional<Texture> height = std::optional<Texture>{};
	};

	mat(const std::vector<InitData>& materials);

private:
	struct UniformData {
		glm::vec3 diffuse;
		float opacity;
		glm::vec3 specular;
		float shininess;
		glm::vec3 emissive;
		float ambientOcclusion;

		uint64_t diffuseMap;
		uint64_t opacityMap;
		uint64_t specularMap;
		uint64_t shininessMap;
		uint64_t emissiveMap;
		uint64_t ambientOcclusionMap;
		uint64_t normalMap;
		uint64_t heightMap;

		int diffuseHasMap;
		int opacityHasMap;
		int specularHasMap;
		int shininessHasMap;
		int emissiveHasMap;
		int ambientOcclusionHasMap;
		int normalHasMap;
		int heightHasMap;
	};

	InitData* m_initializerData;
	std::vector<unsigned int> m_materialIndices;
};

class MaterialVBO : Buffer {
private:
	unsigned int m_count;
};

template<unsigned int materialIndex = 0>
class MaterialTEST {



};