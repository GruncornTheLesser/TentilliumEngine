#pragma once
#include "Resource.h"
#include <iostream>
#include <string>
struct Texture : Resource<Texture>
{
private:
	unsigned int m_handle;

public:
	Texture(std::string filepath);
	Texture(unsigned int width, unsigned int height, unsigned int channels, void* data = nullptr);

	~Texture();

	int getWidth();
	int getHeight();

	void bind() const;
};