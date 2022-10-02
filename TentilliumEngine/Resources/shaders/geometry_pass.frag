#version 460 core

#extension GL_ARB_bindless_texture : require

struct Material {
	vec3 diffuse;
	float opacity;
	vec3 specular;
	float shininess;
	vec3 emissive;
	float ambientOcclusion;

	sampler2D diffuseMap;
	sampler2D opacityMap;
	sampler2D specularMap;
	sampler2D shininessMap;
	sampler2D emissiveMap;
	sampler2D ambientOcclusionMap;
	sampler2D normalMap;
	sampler2D heightMap;

	bool diffuseHasMap;
	bool opacityHasMap;
	bool specularHasMap;
	bool shininessHasMap;
	bool emissiveHasMap;
	bool ambientOcclusionHasMap;
	bool normalHasMap;
	bool heightHasMap;
};

in VERTEX_OUT {
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
	flat uint materialIndex;
} fragment_in;


layout(location = 0) out vec3 positionAttachment; 
layout(location = 1) out vec3 normalAttachment;
layout(location = 2) out vec2 TexCoordAttachment;
layout(location = 3) out uint MaterialAttachment;

layout(std140, binding = 0) uniform MaterialBuffer {
	Material materials[256];
};

vec3 getPosition() {
	return fragment_in.position;
}

vec3 getNormal() {
	vec3 normal = normalize(fragment_in.normal);
	
	if (!materials[fragment_in.materialIndex].normalHasMap) return normal;

	vec3 tangent = normalize(fragment_in.tangent);
	vec3 bitangent = cross(tangent, normal);
	mat3 TBN = mat3(tangent, bitangent, normal);

	return normalize(TBN * (2 * texture(materials[fragment_in.materialIndex].normalMap, fragment_in.texcoord).xyz - 1));
}

void main() {
	positionAttachment = getPosition();
	normalAttachment = getNormal();
	TexCoordAttachment = fragment_in.texcoord;
	MaterialAttachment = fragment_in.materialIndex;
};