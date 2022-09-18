#version 460 core

struct PointLight {
	vec4 position;
	vec4 color;
	vec4 radius;
};

struct AABB {		// axis aligned bounding box
	vec4 minPoint;	// + padding
	vec4 maxPoint;	// + padding
};

struct LightArray {
	uint index;
	uint count;
};

in VERTEX_OUT {
	vec3 position;
	vec2 texcoord;
	vec3 normal;
} fragment_in;

out vec4 fragColour;

uniform sampler2D sceneDepthMap;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;

uniform mat4 MVP;
uniform vec3 viewPosition;

// buffers
layout (std430, binding = 0) readonly buffer RenderData {
    mat4 invProj;
	float zNear;
	float zFar;
	uvec2 screenSize;
	ivec3 clusterCount;
};

layout(std430, binding = 1) readonly buffer ClusterBuffer {
	AABB clusters[];
};

layout(std430, binding = 2) readonly buffer LightBuffer {
	PointLight lights[];
};

layout(std430, binding = 3) writeonly buffer IndicesBuffer {
	uint lightIndices[];
};

layout(std430, binding = 4) writeonly buffer LightArrayBuffer {
	LightArray lightArrays[];
};

layout (std430, binding = 5) readonly buffer Material {
	vec4 diffuse;			// align 16,	size 16,	offset 0
	vec4 specular;			// align 16,	size 16,	offset 16
	bool hasDiffuseMap;		// align 4,		size 4,		offset 20
	bool hasSpecularMap;	// align 4,		size 4,		offset 24
	bool hasNormalMap;		// align 4,		size 4,		offset 28
};

// functions
float attenuate(vec3 lightDirection, float radius) {
// Attenuate the point light intensity
	float cutoff = 0.5;
	float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
	attenuation = 1.0 / (attenuation * 15.0 + 1.0);
	attenuation = (attenuation - cutoff) / (1.0 - cutoff);

	return clamp(attenuation, 0.0, 1.0);
}

float linearDepth(float depthSample) {
    float depthRange = 2.0 * depthSample - 1.0;
    float linear = 2.0 * zNear * zFar / (zFar + zNear - depthRange * (zFar - zNear));
    return linear;
}

void main() {
	// Get color and normal components from texture maps
	vec4 base_diffuse =	 hasDiffuseMap  ? texture(diffuseMap,  fragment_in.texcoord) : diffuse;
	vec4 base_specular = hasSpecularMap ? texture(specularMap, fragment_in.texcoord) : specular;
	vec3 normal =		 hasNormalMap   ? texture(normalMap,   fragment_in.texcoord).rgb : vec3(0, 0, 1);;

	//uint zTile = uint(max(linearDepth(gl_FragCoord.z), 0.0));

	fragColour = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1);

	//vec3 viewDir = normalize(viewPosition - fragment_in.position);
    //vec3 R = reflect(-viewDir, normal);
	//
	//uint lightCount       = lightClusters[tileIndex].count;
    //uint lightIndexOffset = lightClusters[tileIndex].offset;

}