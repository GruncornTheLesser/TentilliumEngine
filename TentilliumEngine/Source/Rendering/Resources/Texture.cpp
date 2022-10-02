#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths

#include <exception>
#include <iostream>

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

	setWrap(wrap);
	setFilter(filter);

	// generate opengl texture from bitmap, internal_format = None means internal_format == data_format and vice versa
	setData(width, height, Format::NONE, data, (Format)data_format);

	// release stbi image data
	stbi_image_free(data);

}

Texture::Texture(int width, int height, Format internal_format_hint, void* data, Format data_format_hint, Wrap wrap, Filter filter)
{
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);

	setWrap(wrap);
	setFilter(filter);

	setData(width, height, internal_format_hint, data, data_format_hint);

}

void Texture::setData(int width, int height, Format internal_format_hint, void* data, Format data_format_hint)
{
	bool compressed = height == 0 || width == 0;

	if (compressed) {
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load_from_memory((unsigned char*)data, std::max(1, width) * std::max(1, height), &width, &height, (int*)(&data_format_hint), 0);

		if (!data) {
			std::cerr << "[Loading Error] - Failed decompress image: " << stbi_failure_reason() << std::endl;
			throw std::exception();
		}
	}

	glBindTexture(GL_TEXTURE_2D, m_handle);

	int internal_format, data_format, type;

	if (data_format_hint == Format::NONE && internal_format_hint == Format::NONE) 
	{
		internal_format_hint = Format::RGBA;
		data_format_hint = Format::RGBA;
	}
	
	if (internal_format_hint == Format::NONE)
		internal_format_hint = data_format_hint;

	if (data_format_hint == Format::NONE)
		data_format_hint = internal_format_hint;

	switch (internal_format_hint) {
		case Format::R: internal_format = GL_RED; break;
		case Format::RG: internal_format = GL_RG; break;
		case Format::RGB: internal_format = GL_RGB; break;
		case Format::RGBA:	internal_format = GL_RGBA; break;

		case Format::R_16F: internal_format = GL_R16F; break;
		case Format::RG_16F: internal_format = GL_RG16F; break;
		case Format::RGB_16F: internal_format = GL_RGB16F; break;
		case Format::RGBA_16F: internal_format = GL_RGBA16F; break;

		case Format::R_16I: internal_format = GL_R16I; break;
		case Format::RG_16I: internal_format = GL_RG16I; break;
		case Format::RGB_16I: internal_format = GL_RGB16I; break;
		case Format::RGBA_16I: internal_format = GL_RGBA16I; break;

		case Format::R_16UI: internal_format = GL_R16UI; break;
		case Format::RG_16UI: internal_format = GL_RG16UI; break;
		case Format::RGB_16UI: internal_format = GL_RGB16UI; break;
		case Format::RGBA_16UI: internal_format = GL_RGBA16UI; break;

		case Format::R_32F: internal_format = GL_R16F; break;
		case Format::RG_32F: internal_format = GL_RG16F; break;
		case Format::RGB_32F: internal_format = GL_RGB16F; break;
		case Format::RGBA_32F: internal_format = GL_RGBA16F; break;

		case Format::R_32I: internal_format = GL_R32I; break;
		case Format::RG_32I: internal_format = GL_RG32I; break;
		case Format::RGB_32I: internal_format = GL_RGB32I; break;
		case Format::RGBA_32I: internal_format = GL_RGBA32I; break;

		case Format::R_32UI: internal_format = GL_R32UI; break;
		case Format::RG_32UI: internal_format = GL_RG32UI; break;
		case Format::RGB_32UI: internal_format = GL_RGB32UI; break;
		case Format::RGBA_32UI: internal_format = GL_RGBA32UI; break;

		case Format::DEPTH: internal_format = GL_DEPTH_COMPONENT; break;
	}

	switch (data_format_hint) {
	case Format::R:
	case Format::R_16F:
	case Format::R_32F:data_format = GL_RED; break;

	case Format::R_16I:
	case Format::R_16UI:
	case Format::R_32I:
	case Format::R_32UI: data_format = GL_RED_INTEGER; break;

	case Format::RG:
	case Format::RG_16F:
	case Format::RG_32F: data_format = GL_RG; break;

	case Format::RG_16I:
	case Format::RG_16UI:  
	case Format::RG_32I:
	case Format::RG_32UI: data_format = GL_RG_INTEGER;

	case Format::RGB:
	case Format::RGB_16F:
	case Format::RGB_32F: data_format = GL_RGB; break;

	case Format::RGB_16I:
	case Format::RGB_16UI:
	case Format::RGB_32I:
	case Format::RGB_32UI: data_format = GL_RGB_INTEGER; break;

	case Format::RGBA:
	case Format::RGBA_16F:
	case Format::RGBA_32F: data_format = GL_RGBA; break;

	case Format::RGBA_16I:
	case Format::RGBA_16UI:
	case Format::RGBA_32I:
	case Format::RGBA_32UI: data_format = GL_RGBA_INTEGER; break;

	case Format::DEPTH: data_format = GL_DEPTH_COMPONENT; break;
	}

	switch (data_format_hint) {
	case Format::R:
	case Format::RG:
	case Format::RGB:
	case Format::RGBA:		type = GL_UNSIGNED_BYTE; break;
	
	case Format::R_16F:
	case Format::RG_16F:
	case Format::RGB_16F:
	case Format::RGBA_16F: type = GL_HALF_FLOAT; break;

	case Format::R_16I:
	case Format::RG_16I:
	case Format::RGB_16I:
	case Format::RGBA_16I: type = GL_SHORT; break;

	case Format::R_16UI:
	case Format::RG_16UI:
	case Format::RGB_16UI:
	case Format::RGBA_16UI: type = GL_UNSIGNED_SHORT; break;

	case Format::R_32F:
	case Format::RG_32F:
	case Format::RGB_32F:
	case Format::RGBA_32F: type = GL_FLOAT; break;

	case Format::R_32I: 
	case Format::RG_32I: 
	case Format::RGB_32I: 
	case Format::RGBA_32I: type = GL_INT; break;

	case Format::R_32UI: 
	case Format::RG_32UI: 
	case Format::RGB_32UI: 
	case Format::RGBA_32UI: type = GL_UNSIGNED_INT; break;

	case Format::DEPTH: type = GL_FLOAT; break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, type, data);

	glBindTexture(GL_TEXTURE_2D, NULL);

	if (compressed) 
		stbi_image_free(data);
}

void Texture::setBindless(bool value)
{
	std::uint64_t handle = glGetTextureHandleARB(m_handle);
	if (glIsTextureHandleResidentARB(handle) != (GLboolean)value)
	{
		if (value) glMakeTextureHandleResidentARB(handle);
		else	   glMakeTextureHandleNonResidentARB(handle);
	}
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
	case GL_CLAMP_TO_EDGE:		return Texture::Wrap::CLAMP_EDGE;
	case GL_MIRRORED_REPEAT:	return Texture::Wrap::MIRRORED_REPEAT;
	case GL_REPEAT:				return Texture::Wrap::REPEAT;
	}
	return (Texture::Wrap)wrap;
}

void Texture::setWrap(Texture::Wrap wrap)
{
	int glWrap;
	switch (wrap) {
	case Texture::Wrap::CLAMP_EDGE:			glWrap = GL_CLAMP_TO_EDGE; break;
	case Texture::Wrap::MIRRORED_REPEAT:	glWrap = GL_MIRRORED_REPEAT; break;
	case Texture::Wrap::REPEAT:				glWrap = GL_REPEAT; break;
	}

	glBindTexture(GL_TEXTURE_2D, m_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
	glBindTexture(GL_TEXTURE_2D, NULL);
}

Texture::Filter Texture::getFilter() const
{
	int filter;
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &filter);
	switch (filter) {
	case GL_LINEAR:		return Texture::Filter::LINEAR;
	case GL_NEAREST:	return Texture::Filter::NEAREST;
	}
	return (Texture::Filter)wrap;
}

void Texture::setFilter(Texture::Filter filter)
{
	int glFilter;
	switch (filter) {
	case Texture::Filter::LINEAR:	glFilter = GL_LINEAR; break;
	case Texture::Filter::NEAREST:	glFilter = GL_NEAREST; break;
	}

	glBindTexture(GL_TEXTURE_2D, m_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
	glBindTexture(GL_TEXTURE_2D, NULL);
}

void Texture::destroy(unsigned int handle)
{
	glDeleteTextures(1, &handle);
}