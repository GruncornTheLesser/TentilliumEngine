#pragma once
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include "Flag.h"


struct Transform
{
public:
	__declspec(property (put = setPosition, get = getPosition)) glm::vec3& position;
	__declspec(property (put = setRotation, get = getRotation)) glm::quat& rotation;
	__declspec(property (put = setScale, get = getScale)) glm::vec3& scale;

	friend class Scene;
private:
	Flag m_localUpdateFlag;
	Flag m_worldUpdateFlag;

	glm::vec3 m_scale;
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::mat4 m_localMatrix;
	glm::mat4 m_worldMatrix;

public:
	Transform(const glm::mat4& localMatrix);
	Transform(const glm::vec3& position = glm::vec3(0, 0, 0), 
			  const glm::vec3& scale = glm::vec3(1),
			  const glm::quat& rotation = glm::quat(glm::vec3(0, 0, 0)));

public:
	const glm::vec3& getPosition();
	const glm::vec3& getScale();
	const glm::quat& getRotation();
	const glm::mat4& getLocalMatrix();

	void setPosition(const glm::vec3& pos);
	void setScale(const glm::vec3& scl);
	void setRotation(const glm::quat& rot);
	void setRotation(const glm::vec3& rot);

	void updateLocal(Transform* parent);
	void updateLocal();

public:
	static void Decompose(glm::mat4 mat, glm::vec3& pos, glm::vec3& sca, glm::quat& rot);
	operator const glm::mat4& () { return m_worldMatrix; }
};

