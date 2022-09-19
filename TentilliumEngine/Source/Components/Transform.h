#pragma once
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/transform.hpp>
#include <gtc/quaternion.hpp>


struct Position {
	friend class TransformSystem;
public:
	Position() { }

	template<typename t_x, typename t_y, typename t_z>
	Position(const t_x& x, const t_y& y, const t_z& z)
		: m_position(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z))
	{ }

	template<typename T, glm::precision P>
	Position(const glm::tvec3<T, P>& position)
		: m_position(position)
	{ }

	glm::mat4 getMatrix() { return glm::translate(m_position); }
	operator glm::vec3() { return m_position; }

private:
	glm::vec3 m_position;
};

struct Scale {
	friend class TransformSystem;
public:
	Scale() { }

	template<typename t_x, typename t_y, typename t_z>
	Scale(const t_x& x, const t_y& y, const t_z& z)
		: m_scale(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z))
	{ }
	
	template<typename t>
	Scale(const t& s)
		: m_scale(s, s, s)
	{ }

	template<typename T, glm::precision P>
	Scale(const glm::tvec3<T, P>& scale)
		: m_scale(scale)
	{ }

	glm::mat4 getMatrix() { return glm::scale(m_scale); }
	operator glm::vec3() { return m_scale; }

private:
	glm::vec3 m_scale;
};

struct Rotation {
	friend class TransformSystem;
public:
	Rotation() { }

	template<typename T, glm::precision P>
	Rotation(const glm::tquat<T, P>& rotation)
		: m_rotation(rotation)
	{ }

	template<typename T, glm::precision P>
	Rotation(const glm::tvec3<T, P>& eulerAngles)
		: m_rotation(eulerAngles)
	{ }
	template<typename t_x, typename t_y, typename t_z>
	Rotation(const t_x& euler_x, const t_y& euler_y, const t_z& euler_z)
		: m_rotation(glm::vec3(static_cast<float>(euler_x), static_cast<float>(euler_y), static_cast<float>(euler_z)))
	{ }

	glm::mat4 getMatrix() { return (glm::mat4)m_rotation; }
	glm::vec3 getEulerAngles() { return glm::eulerAngles(m_rotation); }
	operator glm::quat() { return m_rotation; }

private:
	glm::quat m_rotation;
};

struct LocalTransform {
	friend class TransformSystem;
public:
	LocalTransform()
		: m_transform()
	{ }

	template<typename T, glm::precision P>
	LocalTransform(const glm::tmat4x4<T, P>& transform)
		: m_transform(transform)
	{ }

	operator glm::mat4() { return m_transform; }

private:
	glm::mat4 m_transform;
};

struct WorldTransform {
	friend class TransformSystem;
public:
	WorldTransform()
		: m_transform()
	{ }

	template<typename T, glm::precision P>
	WorldTransform(const glm::tmat4x4<T, P>& transform)
		: m_transform(transform)
	{ }

	glm::vec3 getPosition() { return glm::vec3(m_transform[3]); }
	
	operator glm::mat4() { return m_transform; }

private:
	glm::mat4 m_transform;
};
