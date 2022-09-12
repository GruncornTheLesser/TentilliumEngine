#version 460 core
// vertex
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 MVP;

out VERTEX_OUT {
	vec3 position;
	vec2 texcoord;
	vec3 normal;
} vertex_out;

void main() {
	gl_Position = MVP * vec4(position, 1);
	vertex_out.texcoord = texCoord;
	vertex_out.normal = normal;
};