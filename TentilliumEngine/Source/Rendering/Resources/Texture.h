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
	enum class Format { NONE, R, RG, RGB, RGBA, DEPTH, STENCIL };
	enum class Type { UNSIGNED_INT, UNSIGNED_BYTE, FLOAT };

	__declspec(property (get=getHandle)) unsigned int handle;
	__declspec(property (get=getSize)) glm::ivec2 size;

	__declspec(property (put=setWrap, get=getWrap)) Wrap wrap;
	__declspec(property (put=setFilter, get=getFilter)) Filter filter;

public:
	Texture();

	Texture(std::string filepath, Wrap wrap = Wrap::CLAMP_EDGE, Filter filter = Filter::NEAREST);

	Texture(int width, int height, 
		Format internal_format_hint, 
		bool normalized = true, 
		void* data = nullptr, 
		Format data_format_hint = Format::NONE,
		Type type_hint = Type::UNSIGNED_BYTE, 
		Wrap wrap = Wrap::CLAMP_EDGE,
		Filter filter = Filter::NEAREST
	);

	void setData(int width, int height, 
		Format internal_format_hint, 
		bool normalized = true, 
		void* data = nullptr, 
		Format data_format_hint = Format::NONE, 
		Type type_hint = Type::UNSIGNED_BYTE);

	void bindSlot(unsigned int slot) const;

	glm::ivec2 getSize() const;

	unsigned int getHandle() const { return m_handle; }

	Wrap getWrap() const;
	void setWrap(Wrap wrap) const;

	Filter getFilter() const;
	void setFilter(Filter filter) const;

private:
	static void destroy(unsigned int handle);
};