#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
using namespace glm;

struct Transform
{
public:
	vec3 scale;
	vec3 position;
	quat rotation;
	mat4 localMatrix;
	mat4 worldMatrix;
public:
	Transform() {}
	Transform(vec3 position, vec3 scale = vec3(1), quat rotation = quat(1, 0, 0, 0)) :
		scale(scale),
		position(position),
		rotation(rotation)
	{ }
	operator const mat4& () { return worldMatrix; }

};

