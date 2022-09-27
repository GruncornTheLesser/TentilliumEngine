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

out vec4 f_colour;

uniform sampler2D fboAttachment0;	// position + specular
uniform sampler2D fboAttachment1;	// normal + gloss
uniform sampler2D fboAttachment2;	// diffuse + ao

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

void main() {
	vec2 uv = gl_FragCoord.xy / screenSize.xy;

	vec4 attachment0 = texture(fboAttachment0, uv); // position + depth
	vec4 attachment1 = texture(fboAttachment1, uv); // normal + gloss
	vec4 attachment2 = texture(fboAttachment2, uv); // diffuse + specular

	// geometry data
	vec3 position = attachment0.rgb;
	float depth = attachment0.a;
	vec3 normal = attachment1.rgb;
	
	// material data
	float gloss = attachment1.a;
	vec3 diffuse = attachment2.rgb;
	float specular = attachment2.a;
	
	
	uvec3 ID = getClusterID(depth);
	
	uint clusterIndex = ID.x + ID.y * clusterSize.x + ID.z * clusterSize.x * clusterSize.y;

	LightArray lightArray = lightArrays[clusterIndex];

	vec3 ambiance = vec3(0.1, 0.1, 0.1);
	vec3 radiance = vec3(0.0, 0.0, 0.0);
	for (uint i = lightArray.begin; i < lightArray.end; i++) 
	{
		PointLight light = lights[lightIndices[i]];
		radiance += light.colour.xyz * attenuate(position.xyz - light.position.xyz, light.radius.r);
	}
	f_colour =  vec4(diffuse.xyz * ambiance, 1);
	f_colour += vec4(diffuse.xyz * radiance, 1);
	
}