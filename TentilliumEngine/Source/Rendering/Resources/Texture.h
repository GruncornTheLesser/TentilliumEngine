#pragma once
#include "Resource.h"
#include <glm.hpp>	// GL maths
#include <iostream>
#include <string>
#include <unordered_map>


class Texture : public Resource<Texture>
{
	friend class Material;
	friend class Resource<Texture>;
	friend class Framebuffer;
public:
	enum class Filter { NEAREST, LINEAR };
	enum class Wrap { CLAMP_EDGE, REPEAT, MIRRORED_REPEAT };
	enum class Format { NONE, R, RG, RGB, RGBA, DEPTH, STENCIL };
	enum class Type { NONE, UNSIGNED_INT, UNSIGNED_BYTE, FLOAT };

	__declspec(property (get = getHandle)) unsigned int handle;
	__declspec(property (get = getSize)) glm::ivec2 size;
	__declspec(property (put = setWrap, get = getWrap)) Wrap wrap;
	__declspec(property (put = setFilter, get = getFilter)) Filter filter;

public:
	Texture() : m_handle(0) 
	{ 
	
	}
	Texture(std::string filepath);

	Texture(void* data, int width, int height = 0, Format channels = Format::NONE, Format format_hint = Format::NONE, Type type_hint = Type::NONE); // defaults to unsigned byte

	~Texture();

	Texture(const Texture& other);
	Texture& operator=(const Texture& other);

	Texture(Texture&& other);
	const Texture& operator=(Texture&& other);

	glm::ivec2 getSize() const;

	unsigned int getHandle() const { return m_handle; }

	Wrap getWrap() const;
	void setWrap(Wrap wrap) const;

	Filter getFilter() const;
	void setFilter(Filter filter) const;

private:
	unsigned int m_handle;
	Texture(unsigned int handle) : m_handle(handle) { };
};