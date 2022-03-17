#pragma once
#include <glm.hpp>
struct Camera
{
private:
	glm::mat4 proj;
	float fovY, ratio, zNear, zFar;

public:
	Camera(float fovY, float ratio, float zNear, float zFar);

	void resize(int width, int height);

	operator glm::mat4() { return proj; }
};

