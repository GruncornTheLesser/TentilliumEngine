#version 460 core

out vec4 f_colour;

uniform vec3 Colour;

void main() {
	f_colour = vec4(Colour, 1);
};