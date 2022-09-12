#include "Framebuffer.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &m_handle);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_handle);
}

Framebuffer::Framebuffer(const Framebuffer&& other)
{
	this->m_handle = other.m_handle;
}

Framebuffer& Framebuffer::operator=(const Framebuffer&& other)
{
	if (this == &other)
		return *this; // call copy constructor

	glDeleteFramebuffers(1, &m_handle);
	this->m_handle = other.m_handle;
	return *this;
}

void Framebuffer::attach(int attachment, const Texture& texture)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.m_handle, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_attachments[attachment] = texture;
}

void Framebuffer::detach(int attachment)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_attachments.erase(attachment);
}

void Framebuffer::resize(glm::ivec2 size)
{
	for (auto [attachment, texture] : m_attachments) {
		glBindTexture(GL_TEXTURE_2D, texture.handle);
		glTexImage2D(GL_TEXTURE_2D, 0, attachment, size.x, size.y, 0, attachment, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

const Texture& Framebuffer::get(int attachment)
{
	return m_attachments[attachment];
}