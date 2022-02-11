#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
using namespace glm;

struct Transform
{
friend class Scene;
private:
	static inline unsigned int currentVersion = 0;
	unsigned int version;

	vec3 scale;
	vec3 position;
	quat rotation;
	mat4 localMatrix;
	mat4 worldMatrix;

public:
	Transform() :
		scale(1, 1, 1),
		position(0, 0, 0),
		rotation(quat(vec3(0, 0, 0))),
		worldMatrix(),
		localMatrix(),
		version(currentVersion)
	{ }
	Transform(vec3 position = vec3(0, 0, 0), vec3 scale = vec3(1, 1, 1), quat rotation = quat(vec3(0, 0, 0))) :
		scale(scale),
		position(position),
		rotation(rotation),
		worldMatrix(),
		localMatrix(),
		version(currentVersion)
	{ }

	vec3 getPosition() 
	{ 
		return position; 
	}
	vec3 getScale() 
	{ 
		return scale; 
	}
	quat getRotation() 
	{ 
		return rotation; 
	}

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

	static void Decompose(mat4 mat, vec3& pos, vec3& sca, quat& rot)
	{
		pos = vec3(mat[3]);

		sca.x = length(vec3(mat[0][0], mat[1][0], mat[2][0]));
		sca.y = length(vec3(mat[0][1], mat[1][1], mat[2][1]));
		sca.z = length(vec3(mat[0][2], mat[1][2], mat[2][2]));

		mat[0][0] /= sca.x;
		mat[1][0] /= sca.x;
		mat[2][0] /= sca.x;

		mat[0][1] /= sca.y;
		mat[1][1] /= sca.y;
		mat[2][1] /= sca.y;

		mat[0][2] /= sca.z;
		mat[1][2] /= sca.z;
		mat[2][2] /= sca.z;

		rot.w = sqrt(std::max(0.0f, 1 + mat[0][0] + mat[1][1] + mat[2][2])) / 2;
		rot.x = sqrt(std::max(0.0f, 1 + mat[0][0] - mat[1][1] - mat[2][2])) / 2;
		rot.y = sqrt(std::max(0.0f, 1 - mat[0][0] + mat[1][1] - mat[2][2])) / 2;
		rot.z = sqrt(std::max(0.0f, 1 - mat[0][0] - mat[1][1] + mat[2][2])) / 2;

		rot.x = copysign(rot.x, mat[1][2] - mat[2][1]);
		rot.y = copysign(rot.y, mat[2][0] - mat[0][2]);
		rot.z = copysign(rot.z, mat[0][1] - mat[1][0]);
	}



	operator const mat4& () { return worldMatrix; }
};

