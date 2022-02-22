#SHADER VERTEX
#version 460 core
layout(location = 0) in vec3 position; // attribute position
layout(location = 1) in vec2 UV;

layout(std140) uniform CameraBlock
{
	mat4 view;
	mat4 proj;
};
layout(std140) uniform viewportBlock
{
	ivec4 rect;
	// idk more stuff
};
layout(std140) uniform TransformBLock
{
	mat4 transform;
};
uniform mat4 MVP;

out vec2 fragUV;

void main() {
	gl_Position = MVP * vec4(position, 1);
	fragUV = UV;
};
#ENDSHADER


#SHADER FRAGMENT
#version 460 core

uniform sampler2D tex;

in vec2 fragUV;

out vec3 color;

void main() {
	color = vec3(texture(tex, fragUV));
};
#ENDSHADER