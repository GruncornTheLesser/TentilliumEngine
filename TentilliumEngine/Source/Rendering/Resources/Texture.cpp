#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths

#include <exception>
#include <iostream>

Texture::Texture()
{
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);
}

Texture::Texture(std::string filepath, Wrap wrap, Filter filter)
{
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

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

	// generate opengl texture from bitmap, internal_format = None means internal_format == data_format and vice versa
	setData(width, height, Format::NONE, true, data, (Format)data_format, Type::UNSIGNED_BYTE);

	// release stbi image data
	stbi_image_free(data);

	create(m_handle);

	setWrap(wrap);
	setFilter(filter);
}

Texture::Texture(int width, int height, Format internal_format_hint, bool normalized, void* data, Format data_format_hint, Type type_hint, Wrap wrap, Filter filter)
{
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	if (!data) { // if data empty create empty texture
		setData(width, height, internal_format_hint, normalized);
	}
	else if (height == 0 || width == 0) {
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load_from_memory((unsigned char*)data, std::max(1, width) * std::max(1, height), &width, &height, (int*)(&data_format_hint), 0);

		if (!data) {
			std::cerr << "[Loading Error] - Failed decompress image: " << stbi_failure_reason() << std::endl;
			throw std::exception();
		}

		setData(width, height, internal_format_hint,normalized, data, data_format_hint, type_hint);

		stbi_image_free(data);
	}
	else
	{
		setData(width, height, internal_format_hint, normalized, data, data_format_hint, type_hint);
	}

	create(m_handle);
	
	setWrap(wrap);
	setFilter(filter);
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

Texture::Texture(Texture&& other)
{
	if (destroy(m_handle))
		glDeleteTextures(1, &m_handle);

	m_handle = other.m_handle;
	other.m_handle = 0;
}

const Texture& Texture::operator=(Texture&& other)
{
	if (this == &other)
		return *this; // calls copy constructor

	if (destroy(m_handle))
		glDeleteTextures(1, &m_handle);

	m_handle = other.m_handle;
	other.m_handle = 0;
	return *this;
}

void Texture::setData(int width, int height, Format internal_format_hint, bool normalized, void* data, Format data_format_hint, Type data_type_hint)
{
	glBindTexture(GL_TEXTURE_2D, m_handle);

	int internal_format, data_format, type;

	if (data_format_hint == Texture::Format::NONE && internal_format_hint == Texture::Format::NONE) {
		internal_format_hint = Texture::Format::RGBA;
		data_format_hint = Texture::Format::RGBA;
	}
	
	if (internal_format_hint == Texture::Format::NONE)
		internal_format_hint = data_format_hint;

	if (normalized) {
		switch (internal_format_hint) {
		case Texture::Format::R:  internal_format = GL_RED; break;
		case Texture::Format::RG:  internal_format = GL_RG; break;
		case Texture::Format::RGB:  internal_format = GL_RGB; break;
		case Texture::Format::RGBA:  internal_format = GL_RGBA; break;
		case Texture::Format::DEPTH:  internal_format = GL_DEPTH_COMPONENT; break;
		}
	}
	else
	{
		switch (internal_format_hint) {
		case Texture::Format::R:  internal_format = GL_R16F; break;
		case Texture::Format::RG:  internal_format = GL_RG16F; break;
		case Texture::Format::RGB:  internal_format = GL_RGB16F; break;
		case Texture::Format::RGBA:  internal_format = GL_RGBA16F; break;
		case Texture::Format::DEPTH:  internal_format = GL_DEPTH_COMPONENT; break;
		}
	}

	if (data_format_hint == Texture::Format::NONE)
		data_format_hint = internal_format_hint;

	switch (data_format_hint) {
	case Texture::Format::R:  data_format = GL_RED; break;
	case Texture::Format::RG:  data_format = GL_RG; break;
	case Texture::Format::RGB:  data_format = GL_RGB; break;
	case Texture::Format::RGBA:  data_format = GL_RGBA; break;
	case Texture::Format::DEPTH:  data_format = GL_DEPTH_COMPONENT; break;
	}

	switch (data_type_hint) {
	case Texture::Type::UNSIGNED_BYTE:	type = GL_UNSIGNED_BYTE; break;
	case Texture::Type::UNSIGNED_INT:	type = GL_UNSIGNED_INT; break;
	case Texture::Type::FLOAT:			type = GL_FLOAT; break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, type, data);

	glBindTexture(GL_TEXTURE_2D, NULL);
}

void Texture::bindSlot(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_handle);
}

glm::ivec2 Texture::getSize() const
{
	glm::ivec2 size;
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WIDTH, &size.x);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_HEIGHT, &size.y);
	glBindTexture(GL_TEXTURE_2D, 0);
	return size;
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
