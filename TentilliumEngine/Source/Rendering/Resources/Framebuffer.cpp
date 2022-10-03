#include "Framebuffer.h"
#include <glew.h>
#include <glfw3.h>


Framebuffer::Framebuffer(std::vector<std::pair<unsigned int, Texture::Format>> attachments)
	: m_attachments(attachments.size()),
	  m_screenVBO(std::vector<float>{ -1, 1, 0, -1, -1, 0, 1, 1, 0, 1, -1, 0 })
{
	m_screenVAO.attach(Mesh::V_Position, m_screenVBO, 3, GL_FLOAT, false, 0);

	glGenFramebuffers(1, &m_handle);
	glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

	for (auto& [attachmentID, textureFormat] : attachments)
		m_attachments.emplace(std::pair(attachmentID, Texture(1, 1, textureFormat)));

	for (auto& [attachmentID, texture] : m_attachments)
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentID, GL_TEXTURE_2D, texture.handle, 0);

	std::vector<unsigned int> drawBuffers;
	for (auto& [attachmentID, texture] : m_attachments)
		if (attachmentID != GL_DEPTH_ATTACHMENT) 
			drawBuffers.push_back(attachmentID);

	glDrawBuffers(drawBuffers.size(), drawBuffers.data());

	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void Framebuffer::resize(glm::ivec2 size) const
{
	for (auto& [attachmentID, texture] : m_attachments)
		texture.setData(size.x, size.y, texture.format);
}

const Texture& Framebuffer::getAttachment(unsigned int attachmentID) const
{
	return m_attachments.at(attachmentID);
}

void Framebuffer::draw() const
{
	m_screenVAO.draw(GL_TRIANGLE_STRIP);
}

void Framebuffer::destroy(unsigned int handle)
{
	glDeleteFramebuffers(1, &handle);
}
