#version 460 core

in VERTEX_OUT {
	vec3 position;
	vec2 texcoord;
	vec3 normal;
} fragment_in;

layout (std140) uniform Material {
	vec4 diffuse;			// align 16,	size 16,	offset 0
	vec4 specular;			// align 16,	size 16,	offset 16
	bool hasDiffuseMap;		// align 4,		size 4,		offset 20
	bool hasSpecularMap;	// align 4,		size 4,		offset 24
	bool hasNormalMap;		// align 4,		size 4,		offset 28
} material;


uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;

out vec4 f_colour;

void main() {
	vec4 diffuse = material.hasDiffuseMap ? texture(diffuseMap, fragment_in.texcoord) : material.diffuse;
	vec4 specular = material.hasSpecularMap ? texture(specularMap, fragment_in.texcoord) : material.specular;
	vec3 normal = material.hasNormalMap ? texture(normalMap, fragment_in.texcoord).rgb : vec3(0, 0, 1);
	
	f_colour = diffuse;
};