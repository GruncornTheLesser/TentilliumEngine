#version 460 core
// vertex
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 MVP;

out vec2 v_uv;

void main() {
	gl_Position = vec4(position, 1);
	v_uv = texCoord;
};