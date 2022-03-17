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

public:
	Transform(glm::mat4 localMatrix);
	Transform(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(1, 1, 1), glm::quat rotation = glm::quat(glm::vec3(0, 0, 0)));

public:
	const glm::vec3& getPosition();
	const glm::vec3& getScale();
	const glm::quat& getRotation();
	const glm::mat4& getLocalMatrix();

	void setPosition(glm::vec3 pos);
	void setPosition(float x, float y, float z);
	void deltaPosition(glm::vec3 pos);
	void deltaPosition(float x, float y, float z);

	void setScale(glm::vec3 scl);
	void setScale(float x, float y, float z);
	void deltaScale(glm::vec3 scl);
	void deltaScale(float x, float y, float z);

	void setRotation(glm::quat rot);
	void setRotation(glm::vec3 rot);
	void setRotation(float x, float y, float z);
	void deltaRotation(glm::vec3 rot);
	void deltaRotation(float x, float y, float z);

	void point(glm::vec3 dir);
	void pointAt(glm::vec3 point);

public:
	static void Decompose(glm::mat4 mat, glm::vec3& pos, glm::vec3& sca, glm::quat& rot);
	operator const glm::mat4& () { return worldMatrix; }
};

