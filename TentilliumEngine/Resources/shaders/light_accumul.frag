#version 460 core

// fragment
in VERTEX_OUT {
	vec3 position;
	vec2 texcoord;
	vec3 normal;
} fragment_in;

struct PointLight {
	vec3 colour;
	vec3 position;
	float radius; // + padding
};

struct VisibleIndex {
	int index;
};

out vec4 fragColour;

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer {
	PointLight data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

uniform int num_tiles;

uniform vec3 viewPosition;

// Attenuate the point light intensity
float attenuate(vec3 lightDirection, float radius) {
	float cutoff = 0.5;
	float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
	attenuation = 1.0 / (attenuation * 15.0 + 1.0);
	attenuation = (attenuation - cutoff) / (1.0 - cutoff);

	return clamp(attenuation, 0.0, 1.0);
}

void main() {
// Determine which tile this pixel belongs to
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * num_tiles + tileID.x;

	// Get color and normal components from texture maps
	vec4 base_diffuse = texture(texture_diffuse, fragment_in.texcoord);
	vec4 base_specular = texture(texture_specular, fragment_in.texcoord);
	vec3 normal = texture(texture_normal, fragment_in.texcoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec4 colour = vec4(0.0, 0.0, 0.0, base_diffuse.a);

	vec3 viewDirection = normalize(viewPosition - fragment_in.position);

	// The offset is this tile's position in the global array of valid light indices.
	// Loop through all these indices until we hit max number of lights or the end (indicated by an index of -1)
	// Calculate the lighting contribution from each visible point light
	uint offset = index * 1024;
	for (uint i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++) {
		uint lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		PointLight light = lightBuffer.data[lightIndex];

		// Calculate the light attenuation on the pre-normalized lightDirection
		vec3 lightDirection =  light.position - fragment_in.position;
		float attenuation = attenuate(lightDirection, light.radius);

		// Normalize the light direction and calculate the halfway vector
		lightDirection = normalize(lightDirection);
		vec3 halfway = normalize(lightDirection + viewDirection);

		// Calculate the diffuse and specular components of the irradiance, then irradiance, and accumulate onto color
		float diffuse = max(dot(lightDirection, normal), 0.0);
		
		// How do I change the material propery for the spec exponent? is it the alpha of the spec texture?
		float specular = pow(max(dot(normal, halfway), 0.0), 32.0);

		// Hacky fix to handle issue where specular light still effects scene once point light has passed into an object
		if (diffuse == 0.0) {
			specular = 0.0;
		}

		vec3 irradiance = light.colour * ((base_diffuse.rgb * diffuse) + (base_specular.rgb * vec3(specular))) * attenuation;
		colour += vec4(irradiance, 0);
	}

	colour += vec4(base_diffuse.rgb * 0.08, 0.0);

	// Use the mask to discard any fragments that are transparent
	if (base_diffuse.a <= 0.2) {
		discard;
	}
	
	fragColour = colour;
}