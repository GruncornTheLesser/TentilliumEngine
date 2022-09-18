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

void Framebuffer::attach(Attachment attachment, const Texture& texture)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, (int)attachment, GL_TEXTURE_2D, texture.m_handle, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_attachments[attachment] = texture;
}

void Framebuffer::detach(Attachment attachment)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, (int)attachment, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_attachments.erase(attachment);
}

const Texture& Framebuffer::get(Attachment attachment) const
{
	return m_attachments.at(attachment);
}

void Framebuffer::resize(glm::ivec2 size)
{
	for (auto [attachment, texture] : m_attachments) {
		glBindTexture(GL_TEXTURE_2D, texture.handle);
		glTexImage2D(GL_TEXTURE_2D, 0, (int)attachment, size.x, size.y, 0, (int)attachment, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
}

void Framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}
