#pragma once
#include <gtc/matrix_transform.hpp>

struct Camera
{
private:
	glm::mat4 proj;
	float fovY;
	float ratio;
	float zNear;
	float zFar;
public:
	Camera(float fovY, float ratio, float zNear, float zFar)
		: proj(glm::perspective(fovY, ratio, zNear, zFar)), fovY(fovY), ratio(ratio), zNear(zNear), zFar(zFar)
	{ }

	void Bind()
	{

	}

	void  resize(int width, int height)
	{
		ratio = ((float)width) / height;
		proj = glm::perspective(fovY, ratio, zNear, zFar);
	}
};

