#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <exception>
#include <iostream>

Texture::~Texture()
{
	glDeleteTextures(1, &m_handle);
	std::cout << "deleting Texture " << m_handle << std::endl;
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_handle);
}

Texture* Resource<Texture>::Load(std::string filepath)
{
	unsigned int handle;
	int width;
	int height;
	unsigned int format;
	unsigned int min_filter = GL_NEAREST;
	unsigned int mag_filter = GL_NEAREST;

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	
	stbi_set_flip_vertically_on_load(true);
	uint8_t* data = stbi_load(filepath.c_str(), &width, &height, (int*)(&format), 4); // desired no of channels

	if (!data)
	{
		std::cerr << "[Loading Error] - Failed to load texture from: '" << filepath << "'" << std::endl;
		throw std::exception();
	}

	// this only works because 0-15 unsigned int = 0-15 signed int
	// possibly platform dependant
	switch (format) 
	{
	case 1: format = GL_RED; break;
	case 2: format = GL_RG; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	default:
		std::cerr << "thats alota channels ?!" << std::endl;
		throw std::exception(); break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	//glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, NULL);

	return Resource::Emplace(filepath, handle, width, height, format, GL_NEAREST, GL_NEAREST);
}


