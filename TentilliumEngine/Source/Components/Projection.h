#pragma once
#include <glm.hpp>

struct Projection
{
	friend class RenderSystem;
private:
	glm::mat4 m_proj;
	float m_fovY, m_ratio, m_zNear, m_zFar;

public:
	Projection(float fovY, float ratio, float zNear, float zFar);

	glm::mat4& get() {
		return m_proj;
	}
};

