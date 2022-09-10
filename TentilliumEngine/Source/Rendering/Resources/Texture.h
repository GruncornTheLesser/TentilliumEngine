#pragma once
#include "Resource.h"
#include <iostream>
#include <string>
#include <unordered_map>


class Texture : public Resource<Texture>
{
	friend class Material;
	friend class Resource<Texture>;
public:
	enum class Filter { NEAREST, LINEAR };
	enum class Wrap { CLAMP_EDGE, REPEAT, MIRRORED_REPEAT };
	enum class Format { NONE, R, RG, RGB, RGBA, DEPTH };
	enum class Type { NONE, UNSIGNED_INT, UNSIGNED_BYTE, FLOAT };

	__declspec(property (get=getHandle)) unsigned int handle;
	__declspec(property (get=getWidth)) int width;
	__declspec(property (get=getHeight)) int height;
	__declspec(property (put=setWrap, get=getWrap)) Wrap wrap;
	__declspec(property (put=setFilter, get=getFilter)) Filter filter;

private:
	unsigned int m_handle;

	Texture(unsigned int handle);
public:
	Texture(std::string filepath);
	
	Texture(void* data, int width, int height = 0, Format channels = Format::NONE, Format format_hint = Format::NONE, Type type_hint = Type::NONE); // defaults to unsigned byte

	~Texture();

	Texture(Texture const& other);
	Texture& operator=(Texture const& other);

	int getWidth() const;
	int getHeight() const;

	unsigned int getHandle() const { return m_handle; }

	Wrap getWrap() const;
	void setWrap(Wrap wrap) const;

	Filter getFilter() const;
	void setFilter(Filter filter) const;
};




