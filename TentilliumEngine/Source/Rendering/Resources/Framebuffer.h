#pragma once
#include "Texture.h"
#include <glm.hpp>

class Framebuffer
{
public:
	__declspec(property (get = getHandle)) unsigned int handle;
	
	enum Attachment { 
		DEPTH = 0x8d00, 
		STENCIL = 0x8d20, 
		DEPTH_STENCIL = 0x821a, 
		COLOUR0 = 0x8ce0, 
		COLOUR1 = 0x8ce1,
		COLOUR2 = 0x8ce2,
		COLOUR3 = 0x8ce3,
	};

public:
	Framebuffer();
	~Framebuffer();

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;

	Framebuffer(const Framebuffer&& other);
	Framebuffer& operator=(const Framebuffer&& other);

	void attach(Attachment attachment, const Texture& texture);

	void detach(Attachment attachment);

	const Texture& get(Attachment attachment) const;

	void resize(glm::ivec2 size);

	unsigned int getHandle() const { return m_handle; }

	void bind() const;

	static void unbind();

private:
	unsigned int m_handle;
	std::map<Attachment, Texture> m_attachments;

};