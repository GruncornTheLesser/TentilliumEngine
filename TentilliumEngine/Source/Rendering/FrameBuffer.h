#pragma once
#include <exception>
#include "Resources/Texture.h"
struct FrameBuffer
{
private:
	unsigned int FBO;
	int width, height;

public:
	FrameBuffer();
	~FrameBuffer();

	void SetDraw();
	void SetRead();
	void Bind();
	
	void Attach(Texture* texture);
	void Dettach(Texture* texture);

};

