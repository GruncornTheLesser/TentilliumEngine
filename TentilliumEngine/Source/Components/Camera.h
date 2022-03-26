#pragma once
#include <glm.hpp>

struct Camera
{
private:
	glm::mat4 m_proj;
	float m_fovY, m_ratio, m_zNear, m_zFar;

public:
	Camera(float fovY, float ratio, float zNear, float zFar);

	void resize(int width, int height);

	operator glm::mat4() { return m_proj; }
};

