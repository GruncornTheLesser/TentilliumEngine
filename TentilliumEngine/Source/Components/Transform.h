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

	void setLocalMatrix(mat4 mat)
	{
		localMatrix = mat;
		position = vec3(mat[4][1], mat[4][2], mat[4][3]);

		scale.x = length(vec3(mat[1][1], mat[1][2], mat[1][3]));
		scale.y = length(vec3(mat[2][1], mat[2][2], mat[2][3]));
		scale.z = length(vec3(mat[3][1], mat[3][2], mat[3][3]));
	}
	

	static void ExtractRotation(mat4 mat, vec3& pos, vec3& sca, quat& rot)
	{
		pos = vec3(mat[3]);

		// doesnt account for sign of scale
		sca.x = length(vec3(mat[0][0], mat[1][0], mat[2][0]));
		sca.y = length(vec3(mat[0][1], mat[1][1], mat[2][1]));
		sca.z = length(vec3(mat[0][2], mat[1][2], mat[2][2]));

		// normalize scale values
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

