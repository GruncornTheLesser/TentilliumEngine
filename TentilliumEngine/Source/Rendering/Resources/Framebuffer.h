#pragma once
#include "Resource.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "../../Components/Mesh.h"

class Framebuffer : public Shared<Framebuffer> {
	friend class Shared<Framebuffer>;
public:
	__declspec(property(get = getHandle)) unsigned int handle;

	Framebuffer(std::vector<std::pair<unsigned int, Texture::Format>> m_attachments);

	void resize(glm::ivec2 size) const;

	const Texture& getAttachment(unsigned int attachmentID) const;

	void draw() const;
private:
	static void destroy(unsigned int handle);
	
	mutable std::unordered_map<unsigned int, Texture> m_attachments;
	
	Mesh::VAO					m_screenVAO;
	Mesh::VBO<Mesh::V_Position> m_screenVBO;
};