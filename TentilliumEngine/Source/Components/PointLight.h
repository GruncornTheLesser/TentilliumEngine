#pragma once
#include <glm.hpp>	// GL maths

class PointLight {
	friend class RenderSystem;

public:
	PointLight(glm::vec3 position, glm::vec3 colour, float radius)
		: m_position(position), m_colour(colour), m_radius(radius)
	{}
private:
	glm::vec3 m_position;
	int enabled = true;
	glm::vec3 m_colour;
	float m_radius;
};