#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths

#include <exception>
#include <iostream>

unsigned int generateTexture(int width, int height, int channels, void* data, Texture::Format format_hint, Texture::TypeHint type_hint)
{
	unsigned int m_handle;
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	
	int internal_format, format, type;
	switch (channels) {
		case 1:  format = GL_RED; break;
		case 2:  format = GL_RG; break;
		case 3:  format = GL_RGB; break;
		case 4:  format = GL_RGBA; break;
	}
	switch (format_hint) {
		case Texture::Format::R:  internal_format = GL_R8; break;
		case Texture::Format::RG:  internal_format = GL_RG8; break;
		case Texture::Format::RGB:  internal_format = GL_RGB8; break;
		case Texture::Format::RGBA:  internal_format = GL_RGBA8; break;
		case Texture::Format::NONE:  internal_format = format; break;
	}
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

Texture::Texture(unsigned int handle) : m_handle(handle) 
{ 
	create(m_handle);
}

Texture::Texture(std::string filepath)
{
	int width, height, channels;
	
	// stbi and opengl disagree on uv practices, so flip necessary
	stbi_set_flip_vertically_on_load(true);

	// load image from file and decode into bitmap, req_comp = 0 means req_comp == channels
	void* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

	// if load failed
	if (!data) {
		std::cerr << "[Loading Error] - Failed to load texture from: '" << filepath << "': " << stbi_failure_reason() << std::endl;
		throw std::exception();
	}
	// generate opengl texture from bitmap, stbi uses bytes, format = none means format = channels
	m_handle = generateTexture(width, height, channels, data, Format::NONE, TypeHint::UNSIGNED_BYTE);
	// release stbi image
	stbi_image_free(data);

	create(m_handle);
}

Texture::Texture(int width, int height, int channels, void* data = nullptr, Format format_hint, TypeHint type_hint)
{
	if (!data) {
		m_handle = generateTexture(width, height, channels, nullptr, format_hint, type_hint);
		return;
	}

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load_from_memory((unsigned char*)data, std::max(1, width) * std::max(1, height), &width, &height, &channels, 0);

	if (!data)
	{
		std::cerr << "[Loading Error] - Failed to load image: "<< stbi_failure_reason() << std::endl;
		throw std::exception();
	}

	m_handle = generateTexture(width, height, channels, data, format_hint, type_hint);

	stbi_image_free(data);

	create(m_handle);
}

Texture::~Texture() 
{
	if (destroy(m_handle))
		glDeleteTextures(1, &m_handle);
}

Texture::Texture(Texture const& other) 
{
	create(other.m_handle);

	if (destroy(m_handle)) 
		glDeleteTextures(1, &m_handle);

	m_handle = other.m_handle;
}

Texture& Texture::operator=(Texture const& other) 
{
	if (this == &other)
		return *this; // calls copy constructor

	create(other.m_handle);

	if (destroy(m_handle)) 
		glDeleteTextures(1, &m_handle);

	m_handle = other.m_handle;
	return *this;
}

/*
void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_handle);
}

void Texture::bindSlot(int slot) const 
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_handle);
}
*/
int Texture::getWidth() const 
{
	int width;
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WIDTH, &width);
	return width;
}

int Texture::getHeight() const 
{
	int height;
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_HEIGHT, &height);
	return height;
}

Texture::Wrap Texture::getWrap() const
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
	return (Texture::Wrap)wrap;
}

void Texture::setWrap(Texture::Wrap wrap) const
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
}

Texture::Filter Texture::getFilter() const
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
	return (Texture::Filter)wrap;
}

void Texture::setFilter(Texture::Filter filter) const
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
}