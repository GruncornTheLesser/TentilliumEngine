#version 460 core
in vec2 v_uv;
in vec3 v_normal;

layout (std140) uniform material {
	vec3  DIFF_colour;					// align 16,	size 12,	offset 0
	bool has_DIFF_map;					// align 4,		size 4,		offset 12
	
	float SPEC_colour;					// align 4,		size 4,		offset 16
	bool has_SPEC_map;					// align 4,		size 4,		offset 20

	float GLOS_colour;					// align 4,		size 4,		offset 24
	bool has_GLOS_map;					// align 4,		size 4,		offset 28
};


uniform sampler2D DIFF_map;
uniform sampler2D SPEC_map;
uniform sampler2D GLOS_map;

out vec3 f_colour;

void main() {
	float spec = has_SPEC_map ? texture(SPEC_map, v_uv).x   : SPEC_colour;
	float glos = has_GLOS_map ? texture(GLOS_map, v_uv).x   : GLOS_colour;
	vec3  diff = has_DIFF_map ? texture(DIFF_map, v_uv).xyz : DIFF_colour;

	f_colour = vec3(0.1) + diff;
};