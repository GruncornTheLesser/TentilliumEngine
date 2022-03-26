#version 460 core

uniform sampler2D _diffuse_map_;
uniform sampler2D _normal_map_;
uniform sampler2D _specular_map_;
uniform sampler2D _shininess_map_;

in vec2 fragUV;

out vec3 color;

void main() {
	color = vec3(texture(_diffuse_map_, fragUV));
};