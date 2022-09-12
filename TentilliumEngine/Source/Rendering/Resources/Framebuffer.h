#pragma once
#include "Texture.h"
#include <glm.hpp>

class Framebuffer
{
public:
	__declspec(property (get = getHandle)) unsigned int handle;
	
private:
	unsigned int m_handle;
	std::map<int, Texture> m_attachments;

public:
	Framebuffer();
	~Framebuffer();

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;

	Framebuffer(const Framebuffer&& other);
	Framebuffer& operator=(const Framebuffer&& other);

	void attach(int attachment, const Texture& texture);

	void detach(int attachment);

	const Texture& get(int attachment);

	void resize(glm::ivec2 size);

	unsigned int getHandle() const { return m_handle; }
};