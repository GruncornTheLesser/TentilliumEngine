#pragma once
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include "Flag.h"


struct Transform
{
public:
	friend class TransformSystem;

public:
	__declspec(property (put = setPosition, get = getPosition)) const glm::vec3& position;
	__declspec(property (put = setRotation, get = getRotation)) const glm::quat& rotation;
	__declspec(property (put = setScale, get = getScale)) const glm::vec3& scale;

private:
	Flag m_updateFlag;

	glm::vec3 m_scale;
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::mat4 m_localMatrix;
	glm::mat4 m_worldMatrix;

public:
	/*constructs new transform by setting local matrix and decomposing local matrix into position, scale and rotation*/
	Transform(const glm::mat4& localMatrix);
	/*constructs new transform by calculating new local matrix and setting position, scale and rotation*/
	Transform(const glm::vec3& position = glm::vec3(0, 0, 0), 
			  const glm::vec3& scale = glm::vec3(1),
			  const glm::quat& rotation = glm::quat(glm::vec3(0, 0, 0)));

public:
	const glm::vec3& getPosition();
	const glm::vec3& getScale();
	const glm::quat& getRotation();
	const glm::mat4& getLocalMatrix();
	const glm::mat4& getWorldMatrix();

	void setPosition(const glm::vec3& pos);
	void setScale(const glm::vec3& scl);
	void setRotation(const glm::quat& rot);
	
	// updates local matrix 
	void updateLocal();

public:
	static void Decompose(glm::mat4 mat, glm::vec3& pos, glm::vec3& sca, glm::quat& rot);
	operator const glm::mat4& () { return m_worldMatrix; }
};

