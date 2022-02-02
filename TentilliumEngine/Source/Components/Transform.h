#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
using namespace glm;

struct Transform
{
public:
	unsigned int version;
	static inline unsigned int currentVersion = 0;
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

	void setPosition(vec3 pos)
	{
		position = pos;
		version = currentVersion;
	}

	void setScale(vec3 scl)
	{
		scale = scl;
		version = currentVersion;
	}

	void setRotation(quat rot)
	{
		rotation = rot;
		version = currentVersion;
	}

	operator const mat4& () { return worldMatrix; }

};

