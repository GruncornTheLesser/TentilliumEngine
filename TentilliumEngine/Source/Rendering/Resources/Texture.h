#pragma once
#include "Resource.h"
#include <iostream>
#include <string>
struct Texture : Resource<Texture>
{
	friend class Scene;
private:
	unsigned int m_handle;

	Texture(void* aiTex);
public:
	Texture(std::string filepath);
	Texture(int width, int height, int channels, float* data = nullptr);
	//Texture(int width, int height, int channels, unsigned char* data = nullptr);
	~Texture();

	int getWidth();
	int getHeight();

	void bind() const;
};