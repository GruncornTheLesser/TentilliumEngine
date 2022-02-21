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
	Transform(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(1, 1, 1), glm::quat rotation = glm::quat(glm::vec3(0, 0, 0)));
private:
	void UpdateLocal();
	void UpdateWorld();
	void UpdateWorld(Transform* parent);
public:
	inline glm::vec3 getPosition();
	inline glm::vec3 getScale();
	inline glm::quat getRotation();
	inline glm::mat4 getLocalMatrix();

	inline void setPosition(glm::vec3 pos);
	inline void setScale(glm::vec3 scl);
	inline void setRotation(glm::quat rot);

	static void Decompose(glm::mat4 mat, glm::vec3& pos, glm::vec3& sca, glm::quat& rot);
	operator const glm::mat4& () { return worldMatrix; }
};

