#version 460 core

// vertex
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out VERTEX_OUT {
	vec3 position;
	vec2 texcoord;
	vec3 normal;
} vertex_out;

uniform mat4 model;
uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(position, 1);
	vertex_out.position = vec3(model * vec4(position, 1.0));
	vertex_out.texcoord = texcoord;
	vertex_out.normal = normal;
};