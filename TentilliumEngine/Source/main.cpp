#include "Tentil.h"
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include "OpenGL.h"

using namespace glm;

int main(int argc, char** argv)
{
	AppWindow wnd(640, 480, argv[0]/*the file path*/, true);
	
	mat4 proj_mat = perspective(radians(45.0f), 640.0f / 480.0f, 0.1f, 20.0f);
	mat4 view_mat = lookAt(vec3(5.0f, 5.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)); // position, target, orientation
	mat4 model_mat = mat4(1.0f);
	
	Entity E(
		RenderComponent("Resources/meshes/cube.obj"),
		CustomShader("Resources/shaders/Default.shader")); // custom shader
	
	E.Get<Shader>().SetUniformMatrix4f("u_proj", proj_mat);
	E.Get<Shader>().SetUniformMatrix4f("u_view", view_mat);
	E.Get<Shader>().SetUniformMatrix4f("u_model", model_mat);

	// main loop
	float glfwTime;
	float deltaTime;
	float curtime = 0;
	while (!wnd.Closed())
	{
		// process new frame
		glfwTime = glfwGetTime();
		deltaTime = glfwTime - curtime;
		curtime = glfwTime;

		glClear(GL_COLOR_BUFFER_BIT);

		model_mat = rotate(model_mat, deltaTime, vec3(-1.0f, 0.5f, 0));
		E.Get<Shader>().SetUniformMatrix4f("u_model", model_mat);

		// render system
		entityReg.for_each<Shader, RenderComponent>([](const auto entityID, auto& mesh, auto& shad) {
			shad.Bind();
			mesh.Draw();
		});

		wnd.Refresh();
	}

	glfwTerminate();
	return 0;
}