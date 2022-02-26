#pragma once
#include "Resource.h"
#include <iostream>
struct Texture : Resource<Texture>
{
private:
	int m_width, m_height;
	unsigned int m_handle;
	unsigned int m_format;
	unsigned int m_minFilter;
	unsigned int m_magFilter;

public:
	Texture(unsigned int handle, int width, int height, unsigned int format, unsigned int minFilter, unsigned int magFilter)
		: m_handle(handle), m_width(width), m_height(height), m_format(format), m_minFilter(minFilter), m_magFilter(magFilter)
	{ std::cout << "Creating Texture " << handle << std::endl; }
	~Texture();
	void bind() const;
};