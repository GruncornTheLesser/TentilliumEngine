#pragma once
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include "Flag.h"


struct Transform
{
	friend class Scene;
private:
	Flag localUpdateFlag;
	Flag worldUpdateFlag;

	glm::vec3 scale;
	glm::vec3 position;
	glm::quat rotation;
	glm::mat4 localMatrix;
	glm::mat4 worldMatrix;

	Transform(void* aiTransform) {};
public:
	Transform(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(1, 1, 1), glm::quat rotation = glm::quat(glm::vec3(0, 0, 0)));

public:
	glm::vec3 getPosition();
	glm::vec3 getScale();
	glm::quat getRotation();
	glm::mat4 getLocalMatrix();

	void setPosition(glm::vec3 pos);
	void setScale(glm::vec3 scl);
	void setRotation(glm::quat rot);

public:
	static void Decompose(glm::mat4 mat, glm::vec3& pos, glm::vec3& sca, glm::quat& rot);
	operator const glm::mat4& () { return worldMatrix; }
};

