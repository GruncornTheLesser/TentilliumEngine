#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <exception>
#include <iostream>
#include <assimp/scene.h>

unsigned int Generate_Texture(int width, int height, int channels, void* data, int type_hint, int format_hint)
{
	unsigned int m_handle;
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	int format, internal_format;
	switch (channels)
	{
	case 1: internal_format = GL_R8; format = GL_RED; break;
	case 2: internal_format = GL_RG8; format = GL_RG; break;
	case 3: internal_format = GL_RGB8; format = GL_RGB; break;
	case 4: internal_format = GL_RGBA8; format = GL_RGBA; break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0,
		internal_format, width, height, 0,
		format_hint ? format_hint : format,
		type_hint,
		data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, NULL);

	return m_handle;
}

Texture::Texture(void* aiTex)
{
	aiTexture* texPtr = static_cast<aiTexture*>(aiTex);

	if (texPtr->mHeight == 0) // if embedded compressed texture
	{
		int width, height, channels;
		void* data = stbi_load_from_memory((unsigned char*)(texPtr->pcData), texPtr->mWidth, &width, &height, &channels, 4);

		if (!data)
		{
			std::cerr << "[Loading Error] - Failed to load texture from data" << std::endl;
			throw std::exception();
		}

		m_handle = Generate_Texture(width, height, channels, data, GL_UNSIGNED_BYTE, GL_RGBA);
		
		stbi_image_free(data);
	}
	else if (false)	// if embedded uncompressed texture
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texPtr->mWidth, texPtr->mHeight, 0, GL_BGRA, GL_FLOAT, texPtr->pcData);
	}
	else
	{
					// if non embedded texture
	}
}

Texture::Texture(std::string filepath)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	void* data = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

	if (!data)
	{
		std::cerr << "[Loading Error] - Failed to load texture from: '" << filepath << "'" << std::endl;
		throw std::exception();
	}

	m_handle = Generate_Texture(width, height, channels, data, GL_UNSIGNED_BYTE, GL_RGBA);

	stbi_image_free(data);


}

Texture::Texture(int width, int height, int channels, float* data)
	: m_handle(Generate_Texture(width, height, channels, data, GL_FLOAT, 0)) { }

Texture::~Texture()
{
	glDeleteTextures(1, &m_handle);
}

int Texture::getWidth()
{
	int width;
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WIDTH, &width);
	return width;
}

int Texture::getHeight()
{
	int height;
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_HEIGHT, &height);
	return height;
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_handle);
}




