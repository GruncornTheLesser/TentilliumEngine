#version 460 core

struct PointLight {
	vec4 position;
	vec4 colour;
	vec4 radius;
};

struct AABB {		// axis aligned bounding box
	vec4 minPoint;	// + padding
	vec4 maxPoint;	// + padding
};

struct LightArray {
	uint begin;
	uint end;
};

in VERTEX_OUT {
	vec3 position;
	vec2 texcoord;
	vec3 normal;
} fragment_in;

out vec4 f_colour;

layout(binding = 0) uniform sampler2D diffuseMap;
layout(binding = 1) uniform sampler2D specularMap;
layout(binding = 2) uniform sampler2D normalMap;

uniform mat4 MVP;
uniform vec3 viewPosition;
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

layout(std140) uniform Material {
	vec4 diffuse;			// align 16,	size 16,	offset 0
	vec4 specular;			// align 16,	size 16,	offset 16
	bool hasDiffuseMap;		// align 4,		size 4,		offset 20
	bool hasSpecularMap;	// align 4,		size 4,		offset 24
	bool hasNormalMap;		// align 4,		size 4,		offset 28
} material;

// functions
float attenuate(vec3 lightVector, float radius) {
	// Attenuate the point light intensity
	float dist = length(lightVector);
	float attenuation = pow(clamp(1 - pow((dist / radius), 4.0), 0, 1), 2.0) / (1.0 + (dist * dist));
	return attenuation;
}

uvec3 getClusterID()
{
	uvec3 ID;
	float linear = 2.0 * zNear * zFar / (zFar + zNear - (2.0 * gl_FragCoord.z - 1.0) * (zFar - zNear));
	ID.z = uint(max(log2(linear) * scale + bias, 0.0));
	ID.xy = uvec2(gl_FragCoord.xy / screenSize.xy * clusterSize.xy);
	return ID;
}

void main() {

	// Get color and normal components from texture maps
	vec4 diffuse = material.hasDiffuseMap ? texture(diffuseMap, fragment_in.texcoord) : material.diffuse;
	vec4 specular = material.hasSpecularMap ? texture(specularMap, fragment_in.texcoord) : material.specular;
	vec3 normal = material.hasNormalMap ? texture(normalMap, fragment_in.texcoord).rgb : fragment_in.normal;
	normal = normalize(normal * 2.0 - 1.0);
	
	uvec3 ID = getClusterID();
	
	uint clusterIndex = ID.x + 
						ID.y * clusterSize.x + 
						ID.z * clusterSize.x * clusterSize.y;

	LightArray lightArray = lightArrays[clusterIndex];

	bool edge;
	vec3 radiance = vec3(0.1, 0.1, 0.1);
	for (uint i = lightArray.begin; i < lightArray.end; i++) {
		PointLight light = lights[lightIndices[i]];
		radiance += light.colour.xyz * attenuate(fragment_in.position.xyz - light.position.xyz, light.radius.r);

		float x = light.radius.r - length(fragment_in.position.xyz - light.position.xyz);
		edge = edge || ( 0 < x);
	}
	f_colour = vec4(diffuse.xyz * radiance, 1);

	if (edge)
		f_colour = vec4(1, 0, 1, 1);
	//else
	//	f_colour = vec4(fragment_in.position.xyz, 1);
}