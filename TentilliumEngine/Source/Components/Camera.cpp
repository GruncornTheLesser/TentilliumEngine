#include "Camera.h"
#include <gtc/matrix_transform.hpp>

Camera::Camera(float fovY, float ratio, float zNear, float zFar)
	: m_proj(glm::perspective(fovY, ratio, zNear, zFar)), m_fovY(fovY), m_ratio(ratio), m_zNear(zNear), m_zFar(zFar)
{ }
void Camera::resize(int width, int height)
{
	if (width == 0 || height == 0) return;
	m_ratio = ((float)width) / height;
	m_proj = glm::perspective(m_fovY, m_ratio, m_zNear, m_zFar);
}

