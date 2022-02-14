#pragma once
#include "Resource.h"

struct Texture : Resource<Texture> 
{
private:
	unsigned int handle;

public:
	Texture(unsigned int  hnd) : handle(hnd) { }
	~Texture();
	void bind() const;

};

