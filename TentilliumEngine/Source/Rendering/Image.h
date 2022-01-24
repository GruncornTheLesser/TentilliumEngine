#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <exception>
#include <iostream>
#include "ResourceManager.h"

struct Image {
	friend struct ResourceManager<Image>;
private:
	GLuint handle;
public:
	Image() : handle(0) { }
	~Image() { glDeleteTextures(1, &handle); }
	void Bind() const {
		glBindTexture(GL_TEXTURE_2D, handle);
	}
};

const Image* ResourceManager<Image>::Load(const char* filepath)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	uint8_t* data = stbi_load(filepath, &width, &height, &nrChannels, 4); // desired no of channels

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Failed to load texture" << std::endl;
		throw std::exception();
	}
	stbi_image_free(data);

	auto ptr = &cache[filepath];
	if (ptr->handle)
		glDeleteTextures(1, &(ptr->handle));
	else
		ptr->handle = texture;
	return ptr;
}