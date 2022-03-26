#pragma once
#include "Resource.h"
#include <iostream>
#include <string>
#include <vector>

#define MISSING_TEXTURE Texture::get_or_default("__missing__", 2, 2, 3, std::vector<float>{0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1})




class Texture : public Resource<Texture>
{
public:
	enum class Filter { NEAREST, LINEAR };
	enum class Wrap { CLAMP_EDGE, REPEAT, MIRRORED_REPEAT };

	enum class FormatHint { NONE = 0, R = 1, RG = 2, RGB = 3, RGBA = 4, BGR = 5, BGRA = 6 };
	enum class TypeHint { UNSIGNED_INT, UNSIGNED_BYTE, FLOAT };

public:
	friend class Scene;
	friend class Resource;

	__declspec(property (get=getHandle)) unsigned int handle;
	__declspec(property (get=getWidth)) const int& width;
	__declspec(property (get=getHeight)) const int& height;
	__declspec(property (put=setWrap, get = getWrap)) Wrap wrap;
	__declspec(property (put=setFilter, get = getFilter)) Filter filter;

private:
	unsigned int m_handle;

public:
	Texture(std::string filepath);
	Texture(int width, int height, int channels, std::vector<float> data, FormatHint format_hint = FormatHint::NONE);
	Texture(int width, int height, int channels, std::vector<unsigned int> data, FormatHint format_hint = FormatHint::NONE);
	Texture(int width, int height, int channels, void* data, FormatHint format_hint = FormatHint::NONE, TypeHint type_hint = TypeHint::UNSIGNED_BYTE); // defaults to unsigned int

	~Texture();

	void bind() const;
	void bind(int slot) const;

public:
	int getWidth();
	int getHeight();
	unsigned int getHandle() { return m_handle; }

	Wrap getWrap();
	void setWrap(Wrap wrap);

	Filter getFilter();
	void setFilter(Filter filter);

	
};