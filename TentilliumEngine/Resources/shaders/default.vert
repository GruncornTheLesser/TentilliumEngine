#version 460 core
layout(location = 0) in vec3 position; // attribute position
layout(location = 1) in vec2 UV;

uniform mat4 MVP;

out vec2 fragUV;

void main() {
	gl_Position = MVP * vec4(position, 1);
	fragUV = UV;
};