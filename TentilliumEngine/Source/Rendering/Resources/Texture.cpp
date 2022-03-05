#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <exception>
#include <iostream>
#include <assimp/scene.h>

Texture::Texture(void* aiTex)
{
	aiTexture* texPtr = static_cast<aiTexture*>(aiTex);

	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (texPtr->achFormatHint[0] & 0x01) ? GL_REPEAT : GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (texPtr->achFormatHint[0] & 0x01) ? GL_REPEAT : GL_CLAMP);

	if (texPtr->mHeight == 0) // if embedded compressed texture
	{
		int width, height, channels;
		void* data = stbi_load_from_memory((unsigned char*)(texPtr->pcData), texPtr->mWidth, &width, &height, &channels, 4);
		if (data == nullptr) {
			throw std::exception();
		}

		switch (channels)
		{
		case 1: channels = GL_RED; break;
		case 2: channels = GL_RG; break;
		case 3: channels = GL_RGB; break;
		case 4: channels = GL_RGBA; break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		stbi_image_free(data);
	}
	else if (false)	// if embedded uncompressed texture
	{
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texPtr->mWidth, texPtr->mHeight, 0, GL_BGRA, GL_FLOAT, texPtr->pcData);
	}
	else
	{
					// if non embedded texture
	}

	glBindTexture(GL_TEXTURE_2D, NULL);
}

Texture::Texture(std::string filepath)
{
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	void* data = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

	if (!data)
	{
		std::cerr << "[Loading Error] - Failed to load texture from: '" << filepath << "'" << std::endl;
		throw std::exception();
	}

	switch (channels)
	{
	case 1: channels = GL_RED; break;
	case 2: channels = GL_RG; break;
	case 3: channels = GL_RGB; break;
	case 4: channels = GL_RGBA; break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	//glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, NULL);
}

Texture::Texture(int width, int height, int format)
{
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	switch (format)
	{
	case 1: format = GL_RED; break;
	case 2: format = GL_RG; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, NULL);
}

Texture::Texture(int width, int height, int format, float* data)
{
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	switch (format)
	{
	case 1: format = GL_RED; break;
	case 2: format = GL_RG; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, NULL);
}

Texture::Texture(int width, int height, int format, unsigned int* data)
{
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	switch (format)
	{
	case 1: format = GL_RED; break;
	case 2: format = GL_RG; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_INT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, NULL);
}

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




