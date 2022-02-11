#pragma once
#include "Resource.h"

struct Image : Resource<Image> 
{
private:
	unsigned int handle;

public:
	Image(unsigned int  hnd) : handle(hnd) { }
	~Image();
	void bind() const;

};

