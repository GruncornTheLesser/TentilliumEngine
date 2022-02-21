#include "Transform.h"
#include "FlagManager.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <algorithm>

Transform::Transform(glm::vec3 position, glm::vec3 scale, glm::quat rotation) :
	scale(scale),
	position(position),
	rotation(rotation),
	worldMatrix(1),
	localMatrix(1)
{ 
	worldUpdateFlag.Raise();
	localUpdateFlag.Raise();
}

glm::vec3 Transform::getPosition()
{
	return position;
}

glm::vec3 Transform::getScale()
{
	return scale;
}

glm::quat Transform::getRotation()
{
	return rotation;
}

glm::mat4 Transform::getLocalMatrix()
{
	return localMatrix;
}

void Transform::setPosition(glm::vec3 pos)
{
	position = pos;
	worldUpdateFlag.Raise();
	localUpdateFlag.Raise();

}

void Transform::setScale(glm::vec3 scl)
{
	scale = scl;
	worldUpdateFlag.Raise();
	localUpdateFlag.Raise();
}

void Transform::setRotation(glm::quat rot)
{
	rotation = rot;
	worldUpdateFlag.Raise();
	localUpdateFlag.Raise();
}

void Transform::UpdateLocal()
{
	localMatrix = glm::translate(glm::mat4(1.0f), position);
	localMatrix = glm::scale(localMatrix, scale);
	localMatrix *= glm::mat4(rotation);
}

void Transform::UpdateWorld()
{
	worldMatrix = localMatrix;
}

void Transform::UpdateWorld(Transform* parent)
{
	if (parent)
		worldMatrix = parent->worldMatrix * localMatrix;
	else
		worldMatrix = localMatrix;
}



void Transform::Decompose(glm::mat4 mat, glm::vec3& pos, glm::vec3& sca, glm::quat& rot)
{
	pos = glm::vec3(mat[3]);

	sca.x = glm::length(glm::vec3(mat[0][0], mat[1][0], mat[2][0]));
	sca.y = glm::length(glm::vec3(mat[0][1], mat[1][1], mat[2][1]));
	sca.z = glm::length(glm::vec3(mat[0][2], mat[1][2], mat[2][2]));

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

