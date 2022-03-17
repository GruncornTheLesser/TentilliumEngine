#include "Camera.h"
#include <gtc/matrix_transform.hpp>

Camera::Camera(float fovY, float ratio, float zNear, float zFar)
	: proj(glm::perspective(fovY, ratio, zNear, zFar)), fovY(fovY), ratio(ratio), zNear(zNear), zFar(zFar)
{ 

}
void Camera::resize(int width, int height)
{
	ratio = ((float)width) / height;
	proj = glm::perspective(fovY, ratio, zNear, zFar);
}

