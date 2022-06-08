#include "Transform.h"
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include <gtc/quaternion.hpp>
#include <algorithm>

Transform::Transform(const glm::mat4& matrix) : 
	m_localMatrix(matrix),
	m_worldMatrix(1)
{
	m_localMatrix = matrix;
	Decompose(m_localMatrix, m_position, m_scale, m_rotation);

	m_worldUpdateFlag.Raise();
	m_localUpdateFlag.Lower();
}

Transform::Transform(const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation) :
	m_scale(scale),
	m_position(position),
	m_rotation(rotation),
	m_worldMatrix(1),
	m_localMatrix(1)
{
	m_worldUpdateFlag.Raise();
	m_localUpdateFlag.Raise();
}

const glm::vec3& Transform::getPosition()
{
	return m_position;
}

const glm::vec3& Transform::getScale()
{
	return m_scale;
}

const glm::quat& Transform::getRotation()
{
	return m_rotation;
}

const glm::mat4& Transform::getLocalMatrix()
{
	return m_localMatrix;
}

const glm::mat4& Transform::getWorldMatrix()
{
	return m_worldMatrix;
}

void Transform::setPosition(const glm::vec3& pos)
{
	m_position = pos;
	m_localUpdateFlag.Raise();
}

void Transform::setScale(const glm::vec3& scl)
{
	m_scale = scl;
	m_localUpdateFlag.Raise();
}

void Transform::setRotation(const glm::quat& rot)
{
	m_rotation = rot;
	m_localUpdateFlag.Raise();
}

void Transform::updateLocal()
{
	m_localMatrix =
		glm::translate(m_position) *
		glm::mat4(m_rotation) *
		glm::scale(m_scale);

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