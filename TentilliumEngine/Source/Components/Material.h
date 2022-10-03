#pragma once
#include "../Rendering/Resources/Texture.h"
#include <glm.hpp>
#include <variant>
#include <optional>

#define MAX_MATERIAL_COUNT 256

class Material {
	friend class MaterialSystem;
	friend class LoadSystem;
public:
	struct UniformData {
		glm::vec3 diffuse;
		float opacity;
		glm::vec3 specular;
		float shininess;
		glm::vec3 emissive;
		int padding;

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

	struct Data {
		std::variant<Texture, glm::vec3> diffuse = glm::vec3(1, 1, 1);
		std::variant<Texture, float> opacity = 1.0f;
		std::variant<Texture, glm::vec3> specular = glm::vec3(1, 1, 1);
		std::variant<Texture, float> shininess = 1.0f;
		std::variant<Texture, glm::vec3> emissive = glm::vec3(0, 0, 0);
		std::optional<Texture> ambientOcclusion;
		std::optional<Texture> normal = std::optional<Texture>{};
		std::optional<Texture> height = std::optional<Texture>{};

		UniformData toUniformData();
	};

	Material(std::vector<Data> data)
		: m_data(data), m_indices(data.size())
	{ }

private:
	struct Index : public Shared<Index> {
		friend class Shared<Index>;
	public:
		__declspec(property(get = getHandle)) unsigned int value;
		Index(unsigned int value = 0) { m_handle = value; }
	private:
		static void destroy(unsigned int m_handle) { }
	};

	std::vector<Data> m_data;
	std::vector<Index> m_indices;
};

template<> struct std::hash<Material::Data> {
	size_t operator()(const Material::Data& data) const {
		size_t h = 0;

		if (data.diffuse.index() == 0) {
			const Texture& value = std::get<Texture>(data.diffuse);
			h ^= std::hash<unsigned int>{}(value.handle);
		}
		else {
			const glm::vec3& value = std::get<glm::vec3>(data.diffuse);
			h ^= std::hash<float>{}(value.x);
			h ^= std::hash<float>{}(value.y);
			h ^= std::hash<float>{}(value.z);
		}

		if (data.opacity.index() == 0) {
			const Texture& value = std::get<Texture>(data.opacity);
			h ^= std::hash<unsigned int>{}(value.handle);
		}
		else {
			const float& value = std::get<float>(data.opacity);
			h ^= std::hash<float>{}(value);
		}

		if (data.specular.index() == 0) {
			const Texture& value = std::get<Texture>(data.specular);
			h ^= std::hash<unsigned int>{}(value.handle);
		}
		else {
			const glm::vec3& value = std::get<glm::vec3>(data.specular);
			h ^= std::hash<float>{}(value.x);
			h ^= std::hash<float>{}(value.y);
			h ^= std::hash<float>{}(value.z);
		}

		if (data.shininess.index() == 0) {
			const Texture& value = std::get<Texture>(data.shininess);
			h ^= std::hash<unsigned int>{}(value.handle);
		}
		else {
			const float& value = std::get<float>(data.shininess);
			h ^= std::hash<float>{}(value);
		}

		if (data.emissive.index() == 0) {
			const Texture& value = std::get<Texture>(data.emissive);
			h ^= std::hash<unsigned int>{}(value.handle);
		}
		else {
			const glm::vec3& value = std::get<glm::vec3>(data.emissive);
			h ^= std::hash<float>{}(value.x);
			h ^= std::hash<float>{}(value.y);
			h ^= std::hash<float>{}(value.z);
		}

		if (data.ambientOcclusion.has_value()) {
			const Texture& value = data.ambientOcclusion.value();
			h ^= std::hash<unsigned int>{}(value.handle);
		}

		if (data.height.has_value()) {
			const Texture& value = data.height.value();
			h ^= std::hash<unsigned int>{}(value.handle);
		}

		if (data.normal.has_value()) {
			const Texture& value = data.normal.value();
			h ^= std::hash<unsigned int>{}(value.handle);
		}

		return h;
	}
};

