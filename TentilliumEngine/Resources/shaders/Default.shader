#shader vertex
#version 460 core
// important to include layout in case the attribute is compile out
layout(location = 0) in vec3 a_pos; // attribute position
layout(location = 1) in vec3 a_nor; // attribute normal
layout(location = 2) in vec2 a_tex; // attribute texel

layout(location = 0) out vec2 v_texel;

layout(location = 0) uniform mat4 u_view;
layout(location = 1) uniform mat4 u_proj;
layout(location = 2) uniform mat4 u_model;

void main() {
	v_texel = a_tex;

	gl_Position = u_proj * u_view * u_model * vec4(a_pos, 1);
};

#shader fragment
#version 460 core

layout(location = 0) in vec2 f_texel;

layout(location = 0) out vec4 color;

void main() {
	color = vec4(f_texel, 0, 1);
};