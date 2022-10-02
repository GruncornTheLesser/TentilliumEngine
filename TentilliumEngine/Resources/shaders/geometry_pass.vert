#version 460 core
// vertex
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 texcoord;
//layout(location = 6) in uint material;

out VERTEX_OUT {
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
	flat uint materialIndex;
} vertex_out;

uniform mat4 model;
uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(position, 1);
	vertex_out.texcoord = texcoord;
	vertex_out.position = (model * vec4(position, 1)).xyz;
	vertex_out.tangent = vec3(model * vec4(tangent, 0.0));
	vertex_out.normal = vec3(model * vec4(normal, 0.0));
	vertex_out.materialIndex = 0;

};