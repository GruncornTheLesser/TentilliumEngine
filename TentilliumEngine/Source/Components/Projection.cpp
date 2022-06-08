#include "Projection.h"
#include <gtc/matrix_transform.hpp>

Projection::Projection(float fovY, float ratio, float zNear, float zFar)
	: m_proj(glm::perspective(fovY, ratio, zNear, zFar)), m_fovY(fovY), m_ratio(ratio), m_zNear(zNear), m_zFar(zFar)
{ }


