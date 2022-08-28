#pragma once
#include "entt_macros.h"
#include "../Components/Projection.h"
#include "../Components/Transform.h"
#include <glm.hpp>

#include <gtc/matrix_transform.hpp>

class RenderSystem : virtual protected entt::registry {
public:
	__declspec(property (put = set_camera, get = get_camera)) entt::entity camera;
	void set_camera(entt::entity camera) {
		if (!all_of<Transform, Projection>(camera))
			return;

		m_camera = camera;
	}
	entt::entity get_camera() {
		return m_camera;
	}

protected:
	entt::entity m_camera;

	glm::mat4 get_cam_view() {
		Transform* proj;
		if (valid(m_camera) && (proj = try_get<Transform>(m_camera)))
			return glm::inverse(proj->getWorldMatrix());
		else
			return glm::mat4();
	}

	glm::mat4 get_cam_proj() {
		Projection* view;
		if (valid(m_camera) && (view = try_get<Projection>(m_camera)))
			return view->get();
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



	

};