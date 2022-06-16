#pragma once
#include "entt_macros.h"
#include "../Components/Projection.h"
#include "../Components/Transform.h"
#include <glm.hpp>

#include <gtc/matrix_transform.hpp>

class RenderSystem : virtual protected entt::registry {
protected:
	entt::entity m_camera;

	glm::mat4 get_cam_proj() {
		if (auto ptr = try_get<Transform>(m_camera))
			return glm::inverse(ptr->getWorldMatrix());
		else
			return glm::mat4();
	}

	glm::mat4 get_cam_view() {
		if (auto ptr = try_get<Projection>(m_camera))
			return ptr->get();
		else 
			return glm::mat4();
	}

	void resize(int width, int height) {
		if (width == 0 || height == 0) return;

		if (auto ptr = try_get<Projection>(m_camera))
		{
			ptr->m_ratio = ((float)width) / height;
			ptr->m_proj = glm::perspective(ptr->m_fovY, ptr->m_ratio, ptr->m_zNear, ptr->m_zFar);
		}
	}

	bool set_camera(entt::entity camera) {
		if (!all_of<Transform, Projection>(camera))
			return false;

		m_camera = camera;
		return true;
	}

};