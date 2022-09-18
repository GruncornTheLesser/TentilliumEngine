#pragma once
#include <glm.hpp>	// GL maths

class PointLight {
	friend class RenderSystem;

public:
	PointLight() { }
	PointLight(glm::vec3 position, glm::vec3 colour, float radius)
		: m_position(glm::vec4(position, 0)), m_colour(glm::vec4(colour, 0)), m_radius(glm::vec4(radius, 0, 0, 0))
	{}
private:
	glm::vec4 m_position;
	glm::vec4 m_colour;
	glm::vec4 m_radius;
};