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
	enum class Format { NONE, R, RG, RGB, RGBA };

	enum class TypeHint { UNSIGNED_INT, UNSIGNED_BYTE, FLOAT };

	//enum class TargetHint { TEXTURE2D, TEXTURE3D, } m_target; // ...

public:
	friend class Scene;
	friend class Resource;

	__declspec(property (get=getHandle)) unsigned int handle;
	__declspec(property (get=getWidth)) int width;
	__declspec(property (get=getHeight)) int height;
	__declspec(property (put=setWrap, get = getWrap)) Wrap wrap;
	__declspec(property (put=setFilter, get = getFilter)) Filter filter;

private:
	unsigned int m_handle;

public:
	Texture(std::string filepath);
	Texture(int width, int height, int channels, std::vector<float> data, Format format_hint = Format::NONE);
	Texture(int width, int height, int channels, std::vector<unsigned int> data, Format format_hint = Format::NONE);
	Texture(int width, int height, int channels, void* data, Format format_hint = Format::NONE, TypeHint type_hint = TypeHint::UNSIGNED_BYTE); // defaults to unsigned int

	~Texture();

	Texture(Texture const&) = delete;
	Texture& operator=(Texture const&) = delete;
	Texture(Texture&&);
	Texture& operator=(Texture&&);

	void bind() const;
	void bindSlot(int slot) const;

public:
	int getWidth() const;
	int getHeight() const;

	unsigned int getHandle() { return m_handle; }

	Wrap getWrap();
	void setWrap(Wrap wrap);

	Filter getFilter();
	void setFilter(Filter filter);

	
};