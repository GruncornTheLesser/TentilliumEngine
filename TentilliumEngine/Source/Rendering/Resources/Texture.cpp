#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 
#include "Texture.h"

#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <exception>
#include <iostream>



Texture::~Texture()
{
	glDeleteTextures(1, &handle);
	std::cout << "deleting image " << handle << std::endl;
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, handle);
}


const Texture* Resource<Texture>::Load(const char* filepath)
{
	unsigned int handle;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
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
		stbi_image_free(data);
	}
	else
	{
		std::cerr << "[Loading Error] - Failed to load texture from: '" << filepath << "'" << std::endl;
		throw std::exception();
	}
	
	Resource<Texture>::cache.emplace(filepath, handle);
	return &Resource<Texture>::cache.at(filepath);
}
