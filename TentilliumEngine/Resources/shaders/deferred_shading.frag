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

struct PointLight {
	vec3 position;
	bool enabled;
	vec3 colour;
	float radius;
};

struct AABB {		// axis aligned bounding box
	vec4 minPoint;	// + padding
	vec4 maxPoint;	// + padding
};

struct LightArray {
	uint begin;
	uint end;
};

out vec4 f_colour;

layout(binding = 0) uniform sampler2D positionAttachment;
layout(binding = 1) uniform sampler2D normalAttachment;
layout(binding = 2) uniform sampler2D TexCoordAttachment;
layout(binding = 3) uniform usampler2D MaterialAttachment;
layout(binding = 4) uniform sampler2D DepthAttachment;

uniform vec3 viewPosition;

layout(std140, binding = 0) uniform MaterialBuffer {
	Material materials[256];
};


// buffers
layout(std430, binding = 0) readonly buffer RenderData {
    mat4 invProj;
	uvec3 clusterSize;
	float bias;
	uvec2 screenSize;
	float scale;
	float zNear;
	float zFar;
};

layout(std430, binding = 1) readonly buffer ClusterBuffer {
	AABB clusters[];
};

layout(std430, binding = 2) readonly buffer LightBuffer {
	PointLight lights[];
};

layout(std430, binding = 3) readonly buffer IndicesBuffer {
	uint lightIndices[];
};

layout(std430, binding = 4) readonly buffer LightArrayBuffer {
	LightArray lightArrays[];
};

layout(std430, binding = 5) readonly buffer LightCountBuffer {
	uint visibleCount;
};


// functions
float attenuate(vec3 lightVector, float radius) {
	// Attenuate the point light intensity
	float dist = length(lightVector);
	float attenuation = pow(clamp(1 - pow((dist / radius), 4.0), 0, 1), 2.0) / (1.0 + (dist * dist));
	return attenuation;
}

uvec3 getClusterID(float depth) {
	uvec3 ID;
	
	float linear = 2.0 * zNear * zFar / (zFar + zNear - (2.0 * depth - 1.0) * (zFar - zNear));
	ID.z = uint(max(log2(linear) * scale + bias, 0.0));
	ID.xy = uvec2(gl_FragCoord.xy / screenSize.xy * clusterSize.xy);
	return ID;
}

vec3 getDiffuse(Material mat, vec2 texCoord) {
	if (mat.diffuseHasMap) return texture(mat.diffuseMap, texCoord).rgb;
	else				   return mat.diffuse;
}

vec3 getSpecular(Material mat, vec2 texCoord) {
	if (mat.specularHasMap) return texture(mat.specularMap, texCoord).rgb;
	else				    return mat.specular;
}

float getShininess(Material mat, vec2 texCoord) {
	if (mat.shininessHasMap) return texture(mat.shininessMap, texCoord).r;
	else				     return mat.shininess;
}


void main() {
	vec2 screenUV = gl_FragCoord.xy / screenSize.xy;

	vec3 fragPosition =		texture(positionAttachment, screenUV).rgb; // position
	vec3 N =				texture(normalAttachment, screenUV).rgb;
	vec2 texCoord =			texture(TexCoordAttachment, screenUV).rg;
	uint materialIndex =	texture(MaterialAttachment, screenUV).r;
	float fragDepth =		texture(DepthAttachment, screenUV).r;

	// material data
	vec3 diffuse = getDiffuse(materials[materialIndex], texCoord);
	vec3 specular = getSpecular(materials[materialIndex], texCoord);
	float shininess = getShininess(materials[materialIndex], texCoord);
	
	//f_colour = vec4(texCoord - floor(texCoord), 0, 1);
	//return;
	
	uvec3 ID = getClusterID(fragDepth);	
	uint clusterIndex = ID.x + ID.y * clusterSize.x + ID.z * clusterSize.x * clusterSize.y;
	LightArray lightArray = lightArrays[clusterIndex];

	vec3 radiance = vec3(0.1, 0.1, 0.1);
	for (uint i = lightArray.begin; i < lightArray.end; i++) 
	{
		PointLight light = lights[lightIndices[i]];
		
		// diffuse lighting
		vec3 L = normalize(light.position.xyz - fragPosition);
		vec3 E = normalize(viewPosition - fragPosition);
		vec3 R = reflect(L, N);

		float attn = attenuate(light.position.xyz - fragPosition.xyz, light.radius.r);
		radiance += light.colour.xyz * max(dot(N, L), 0.0) * attn;			// diffuse 
		//radiance += light.colour.xyz * pow(max(dot(R, E), 0.0), 16) * attn; // specular 
	}
	f_colour += vec4(diffuse.xyz * radiance, 1);
}