#version 460 core
// vertex
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 MVP;

out vec2 v_uv;
out vec3 v_normal;
out vec4 v_colour;

void main() {
	gl_Position = MVP * vec4(position, 1);
	v_uv = texCoord;
	v_normal = normal;
};