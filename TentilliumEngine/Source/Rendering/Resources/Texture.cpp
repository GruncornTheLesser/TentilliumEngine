#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths

#include <exception>
#include <iostream>

unsigned int generateTexture(void* data, int width, int height, Texture::Format data_format_hint, Texture::Format internal_format_hint, Texture::Type type_hint)
{
	unsigned int m_handle;
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	int internal_format, data_format, type;
	switch (data_format_hint) {
		case Texture::Format::R:  data_format = GL_RED; break;
		case Texture::Format::RG:  data_format = GL_RG; break;
		case Texture::Format::RGB:  data_format = GL_RGB; break;
		case Texture::Format::RGBA:  data_format = GL_RGBA; break;
		case Texture::Format::DEPTH:  data_format = GL_DEPTH_COMPONENT; break;
	}

	switch (internal_format_hint) {
		case Texture::Format::R:  internal_format = GL_RED; break;
		case Texture::Format::RG:  internal_format = GL_RG; break;
		case Texture::Format::RGB:  internal_format = GL_RGB; break;
		case Texture::Format::RGBA:  internal_format = GL_RGBA; break;
		case Texture::Format::DEPTH: internal_format = GL_DEPTH_COMPONENT; break;
	}

	int selection = 0;
	if (internal_format_hint == Texture::Format::NONE)	selection |= 1;
	if (data_format_hint == Texture::Format::NONE)		selection |= 2;

	switch (selection) {
	case (1):
		internal_format = data_format;
		break;
	case (2):
		data_format = internal_format;
		break;
	case (3):
		std::cerr << "[Input Error] - cannot infer format" << std::endl;
		throw std::exception();
	}

	switch (type_hint) {
		case Texture::Type::UNSIGNED_BYTE:	type = GL_UNSIGNED_BYTE; break;
		case Texture::Type::UNSIGNED_INT:	type = GL_UNSIGNED_INT; break;
		case Texture::Type::FLOAT:			type = GL_FLOAT; break;
		case Texture::Type::NONE:			type = GL_UNSIGNED_BYTE; break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, type, data);

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
	int width, height, data_format;
	
	// stbi and opengl disagree on uv practices, so flip necessary
	stbi_set_flip_vertically_on_load(true);

	// load image from file and decode into bitmap, req_comp = 0 means req_comp == channels
	void* data = stbi_load(filepath.c_str(), &width, &height, &data_format, 0);

	// if load failed
	if (!data) {
		std::cerr << "[Loading Error] - Failed to load texture from: '" << filepath << "': " << stbi_failure_reason() << std::endl;
		throw std::exception();
	}
	
	// generate opengl texture from bitmap, format = NONE => data_format = internal_format
	m_handle = generateTexture(data, width, height, (Format)data_format, Format::NONE, Type::UNSIGNED_BYTE);
	
	// release stbi image
	stbi_image_free(data);

	create(m_handle);
}

Texture::Texture(void* data, int width, int height, Format internal_format_hint, Format data_format_hint, Type type_hint)
{
	// if data nullptr create empty texture
	if (!data) {
		m_handle = generateTexture(nullptr, width, height, data_format_hint, internal_format_hint, type_hint);
		return;
	}

	// if data compressed image, decompress, get data, height, width, data format
	if (height == 0 || width == 0) {
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load_from_memory((unsigned char*)data, std::max(1, width) * std::max(1, height), &width, &height, (int*)(&data_format_hint), 0);
	
		if (!data) {
			std::cerr << "[Loading Error] - Failed decompress image: " << stbi_failure_reason() << std::endl;
			throw std::exception();
		}

		m_handle = generateTexture(data, width, height, data_format_hint, internal_format_hint, type_hint);

		stbi_image_free(data);
	}
	else 
	{
		m_handle = generateTexture(data, width, height, data_format_hint, internal_format_hint, type_hint);
	}

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