#pragma once
#include <glm.hpp>

struct Projection
{
	friend class RenderSystem;

public:
	Projection(float fovY, float ratio, float zNear, float zFar);
	operator glm::mat4() { return m_proj; }
private:
	glm::mat4 m_proj;
	float m_fovY, m_ratio, m_zNear, m_zFar;

};

