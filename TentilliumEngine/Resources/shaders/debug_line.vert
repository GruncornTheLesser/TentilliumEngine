#version 460 core

layout(location = 5) in vec4 position;

uniform mat4 VP;

void main() {
	gl_Position = VP * vec4(position.xyz, 1);
}