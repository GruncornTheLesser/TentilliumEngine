#include "FrameBuffer.h"
#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("frame buffer failed");
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &FBO);
}

void FrameBuffer::SetDraw()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
}

void FrameBuffer::SetRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void FrameBuffer::Attach(Texture* texture)
{
	
}

void FrameBuffer::Dettach(Texture* texture)
{

}