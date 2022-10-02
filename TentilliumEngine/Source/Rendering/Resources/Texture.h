#pragma once
#include "Resource.h"
#include <glm.hpp>	// GL maths
#include <iostream>
#include <string>
#include <unordered_map>


class Texture : public GL<Texture>, public FileManager<Texture>
{
	friend class GL<Texture>;
public:
	enum class Filter { NEAREST, LINEAR };
	enum class Wrap { CLAMP_EDGE, REPEAT, MIRRORED_REPEAT };
	enum class Format {
		NONE,
		R,    	RG,      RGB,      RGBA,    
		R_16F,	RG_16F,  RGB_16F,  RGBA_16F,
		R_16I,	RG_16I,  RGB_16I,  RGBA_16I,
		R_16UI,	RG_16UI, RGB_16UI, RGBA_16UI,
		R_32F,  RG_32F,  RGB_32F,  RGBA_32F,
		R_32I,  RG_32I,  RGB_32I,  RGBA_32I,
		R_32UI, RG_32UI, RGB_32UI, RGBA_32UI,
		DEPTH
	};
	__declspec(property (get=getHandle)) unsigned int handle;
	__declspec(property (get=getSize)) glm::ivec2 size;

	__declspec(property (put=setWrap, get=getWrap)) Wrap wrap;
	__declspec(property (put=setFilter, get=getFilter)) Filter filter;

public:
	Texture(std::string filepath, Wrap wrap = Wrap::CLAMP_EDGE, Filter filter = Filter::NEAREST);

	Texture(int width, int height, 
		Format internal_format_hint, 
		void* data = nullptr, 
		Format data_format_hint = Format::NONE,
		Wrap wrap = Wrap::CLAMP_EDGE,
		Filter filter = Filter::NEAREST
	);

	void setData(int width, int height, 
		Format internal_format_hint, 
		void* data = nullptr, 
		Format data_format_hint = Format::NONE
	);

	void setBindless(bool value);

	void bindSlot(unsigned int slot) const;

	glm::ivec2 getSize() const;

	Wrap getWrap() const;
	void setWrap(Wrap wrap);

	Filter getFilter() const;
	void setFilter(Filter filter);

private:
	static void destroy(unsigned int handle);
};