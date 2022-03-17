#pragma once
#include "Resource.h"
#include <iostream>
#include <string>
struct Texture : Resource<Texture>
{
	friend class Scene;
	friend class Resource;
private:
	unsigned int m_handle;

private:
	Texture(void* texPtr);
public:
	Texture(std::string filepath);
	Texture(int width, int height, int channels);
	Texture(int width, int height, int channels, float* data);
	Texture(int width, int height, int channels, unsigned char* data);
	Texture(int width, int height, int channels, void* data, int hintType, int hintFormat);
	~Texture();

	int getWidth();
	int getHeight();

	void bind() const;
	void bind(int slot) const;
};