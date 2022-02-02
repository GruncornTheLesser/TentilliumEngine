#pragma once

#include <glm.hpp>	
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

#include <gtc/matrix_transform.hpp>
using namespace glm;

#include<entt/entt.hpp>
using namespace entt;
registry privreg;

struct Camera
{
public:
	mat4 proj;
	Camera(float fovY, float ratio, float zNear, float zFar)
		: proj(perspective(fovY, ratio, zNear = 0.1f, zFar = 100.0f)) 
	{ }
};

struct framebuffer {
private:
	GLuint FBO; // frame buffer object
	int width, height;
public:
	void resize(); // needs to update every camera inside it
};

