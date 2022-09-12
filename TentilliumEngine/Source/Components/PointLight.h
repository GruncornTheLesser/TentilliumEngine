#pragma once
#include <glm.hpp>	// GL maths

class PointLight {
public:
	glm::vec3 m_position;
private:
	int padding1;
public:
	glm::vec3 m_color;
private:
	int padding2;
public:
	float m_radius;
private:
	int padding3[3];
public:
	PointLight() { }
	PointLight(glm::vec3 position, glm::vec3 color, float radius)
		: m_position(position), m_color(color), m_radius(radius)
	{}

};