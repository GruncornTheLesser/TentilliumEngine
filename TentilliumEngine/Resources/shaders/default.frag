#version 460 core
in vec2 v_uv;
in vec3 v_normal;

layout (std140) uniform material {
	vec3  diff_colour;					// align 16,	size 12,	offset 0
	bool has_diff_map;					// align 4,		size 4,		offset 12
	
	float spec_colour;					// align 4,		size 4,		offset 16
	bool has_spec_map;					// align 4,		size 4,		offset 20

	float glos_colour;					// align 4,		size 4,		offset 24
	bool has_glos_map;					// align 4,		size 4,		offset 28
};


uniform sampler2D diff_map;
uniform sampler2D spec_map;
uniform sampler2D glos_map;

out vec3 f_colour;

void main() {
	vec3  diff = has_diff_map ? texture(diff_map, v_uv).xyz : diff_colour;
	float spec = has_spec_map ? texture(spec_map, v_uv).x   : spec_colour;
	float glos = has_glos_map ? texture(glos_map, v_uv).x   : glos_colour;
	
	f_colour = vec3(v_uv, 0) + diff;
};