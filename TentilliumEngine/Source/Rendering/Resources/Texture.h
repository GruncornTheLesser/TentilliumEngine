#pragma once
#include "Resource.h"

struct Texture : Resource<Texture>
{
private:
	int m_width, m_height;
	unsigned int m_handle;
	unsigned int m_format;
	unsigned int m_minFilter;
	unsigned int m_magFilter;

public:
	Texture(unsigned int handle, int width, int height, unsigned int format, unsigned int minFilter, unsigned int magFilter);
	~Texture();
	void bind() const;
};