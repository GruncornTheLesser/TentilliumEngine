#pragma once

#include <gtc/matrix_transform.hpp>
using namespace glm;

struct Camera
{
private:
	mat4 proj;
public:
	Camera(float left, float right, float bottom, float top, float zNear = 0.1f, float zFar = 100.0f)
		: proj(frustum(left, right, bottom, top, zNear, zFar)) { }
	

	Camera(float fovY, float ratio, float zNear = 0.1f, float zFar = 100.0f)
		: proj(perspective(fovY, ratio, zNear, zFar)) { }

	void Bind();
};

