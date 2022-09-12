#version 460 core
in vec2 v_uv;

uniform sampler2D map;

out vec3 f_colour;

void main() {
	float depth = texture(map, v_uv).a;
	f_colour = vec3(depth, depth, depth);
};