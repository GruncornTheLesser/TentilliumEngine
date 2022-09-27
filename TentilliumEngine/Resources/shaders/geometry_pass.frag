#version 460 core

in VERTEX_OUT {
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	vec3 tangent;
} fragment_in;


layout(location = 0) out vec4 fboAttachment0; // position + specular
layout(location = 1) out vec4 fboAttachment1;	// normal + gloss
layout(location = 2) out vec4 fboAttachment2;	// diffuse + ao

layout(location = 0) uniform Material {
	vec4 diffuse;
	float specular;
	float gloss;
	bool hasDiffuseMap;
	bool hasSpecularMap;
	bool hasGlossMap;
	bool hasNormalMap;
} material;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D glossMap;
uniform sampler2D normalMap;

vec3 getPosition() {
	return fragment_in.position;
}

vec3 getNormal() {
	vec3 normal = normalize(fragment_in.normal);
	
	if (!material.hasNormalMap) return normal;

	vec3 tangent = normalize(fragment_in.tangent);
	vec3 bitangent = cross(tangent, normal);
	mat3 TBN = mat3(tangent, bitangent, normal);

	return normalize(TBN * (2 * texture(normalMap, fragment_in.texcoord).xyz - 1));
}

vec3 getDiffuse() {
	if (!material.hasDiffuseMap) return material.diffuse.rgb;
	return vec3(texture(diffuseMap, fragment_in.texcoord).rgb);
}

float getSpecular() {
	if (!material.hasSpecularMap) return material.specular;
	else return texture(specularMap, fragment_in.texcoord).r;
}

float getGloss() {
	if (!material.hasGlossMap) return material.gloss;
	else return texture(glossMap, fragment_in.texcoord).r;
}

float getDepth() {
	return gl_FragCoord.z;
}

void main() {
	fboAttachment0 = vec4(getPosition(), getDepth());
	fboAttachment1 = vec4(getNormal(), getGloss());
	fboAttachment2 = vec4(getDiffuse(), getSpecular());
};