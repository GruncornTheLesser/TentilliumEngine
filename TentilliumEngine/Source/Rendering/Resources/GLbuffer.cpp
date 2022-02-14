#include "GLbuffer.h"
#include <glew.h>
#include <glfw3.h>
#include <iostream>

GLbuffer::GLbuffer(unsigned int type, unsigned int length, void* data)
	: target(type), length(length)
{
	glGenBuffers(1, &handle);
	glBindBuffer(target, handle);
	glBufferData(target, length, data, GL_STATIC_DRAW);
	glBindBuffer(target, NULL);
}

void GLbuffer::assign(unsigned int offset, unsigned int length, void* arr)
{
	glBindBuffer(target, handle);
	glBufferSubData(target, offset, length, arr);
	glBindBuffer(target, NULL);
}

char* GLbuffer::get(const unsigned int offset, const unsigned int length)
{
	char* arr = new char[length];

	glBindBuffer(target, handle);
	glGetBufferSubData(target, offset, length, arr);
	glBindBuffer(target, NULL);

	return arr;
}

GLbuffer::~GLbuffer()
{
	glDeleteBuffers(1, &handle);
}
