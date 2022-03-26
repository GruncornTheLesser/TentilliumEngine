#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

#include <exception>
#include <iostream>

#include <assimp/scene.h>

unsigned int generateTexture(int width, int height, int channels, void* data, Texture::FormatHint format_hint, Texture::TypeHint type_hint)
{
	unsigned int m_handle;
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	int format, internal_format;
	if (format_hint != Texture::FormatHint::NONE) 
	{
		switch (format_hint)
		{
			case Texture::FormatHint::R:  internal_format = GL_R8; format = GL_RED; break;
			case Texture::FormatHint::RG:  internal_format = GL_RG8; format = GL_RG; break;
			case Texture::FormatHint::RGB:  internal_format = GL_RGB8; format = GL_RGB; break;
			case Texture::FormatHint::RGBA:  internal_format = GL_RGBA8; format = GL_RGBA; break;
		}
	}
	else
	{
		switch (channels)
		{
		case 1:  internal_format = GL_R8; format = GL_RED; break;
		case 2:  internal_format = GL_RG8; format = GL_RG; break;
		case 3:  internal_format = GL_RGB8; format = GL_RGB; break;
		case 4:  internal_format = GL_RGBA8; format = GL_RGBA; break;
		}
	}

	int type;
	switch (type_hint) {
	case Texture::TypeHint::UNSIGNED_BYTE:	type = GL_UNSIGNED_BYTE; break;
	case Texture::TypeHint::UNSIGNED_INT:	type = GL_UNSIGNED_INT; break;
	case Texture::TypeHint::FLOAT:			type = GL_FLOAT; break;
	}


	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, NULL);

	return m_handle;
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

	m_handle = generateTexture(width, height, channels, data, (FormatHint)channels, TypeHint::UNSIGNED_BYTE);
	stbi_image_free(data);
}

Texture::Texture(int width, int height, int channels, void* data, FormatHint format_hint, TypeHint type_hint)
{
	if (data) 
	{
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load_from_memory((unsigned char*)data, width * (height == 0 ? 1 : height), &width, &height, &channels, 4);

		if (!data)
		{
			std::cerr << "[Loading Error] - Failed to load texture from data" << std::endl;
			throw std::exception();
		}

		m_handle = generateTexture(width, height, channels, data, (format_hint == FormatHint::NONE ? (FormatHint)channels : format_hint), type_hint);

		stbi_image_free(data);
	}
	else
	{
		m_handle = generateTexture(width, height, channels, nullptr, format_hint, type_hint);
	}
		
}

Texture::Texture(int width, int height, int channels, std::vector<float> data, FormatHint format_hint)
	: m_handle(generateTexture(width, height, channels, data.data(), format_hint, TypeHint::FLOAT))
{ }

Texture::Texture(int width, int height, int channels, std::vector<unsigned int> data, FormatHint format_hint)
	: m_handle(generateTexture(width, height, channels, data.data(), format_hint, TypeHint::UNSIGNED_INT))
{ }



Texture::~Texture()
{
	//glDeleteTextures(1, &m_handle);
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_handle);
}

void Texture::bind(int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_handle);
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

Texture::Wrap Texture::getWrap()
{
	int wrap;
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrap);
	switch (wrap) {
	case GL_CLAMP_TO_EDGE: 
		return Texture::Wrap::CLAMP_EDGE;
	case GL_MIRRORED_REPEAT: 
		return Texture::Wrap::MIRRORED_REPEAT;
	case GL_REPEAT: 
		return Texture::Wrap::REPEAT;
	}
}

void Texture::setWrap(Texture::Wrap wrap)
{
	int glWrap;
	glBindTexture(GL_TEXTURE_2D, m_handle);
	switch (wrap) {
	case Texture::Wrap::CLAMP_EDGE: 
		glWrap = GL_CLAMP_TO_EDGE;
		break;
	case Texture::Wrap::MIRRORED_REPEAT: 
		glWrap = GL_MIRRORED_REPEAT;
		break;
	case Texture::Wrap::REPEAT: 
		glWrap = GL_REPEAT;
		break;
	}
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &glWrap);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &glWrap);
}

Texture::Filter Texture::getFilter()
{
	int filter;
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &filter);
	switch (filter) {
	case GL_LINEAR:
		return Texture::Filter::LINEAR;
	case GL_NEAREST:
		return Texture::Filter::NEAREST;
	}
}

void Texture::setFilter(Texture::Filter filter)
{
	int glFilter;
	glBindTexture(GL_TEXTURE_2D, m_handle);
	switch (filter) {
	case Texture::Filter::LINEAR: 
		glFilter = GL_LINEAR; 
		break;
	case Texture::Filter::NEAREST:
		glFilter = GL_NEAREST;
		break;
	}
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &glFilter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &glFilter);
}
