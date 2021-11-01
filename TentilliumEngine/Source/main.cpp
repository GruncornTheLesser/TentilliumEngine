// #include <GLM/glm.hpp>	// mathematics
// #include <GL/glew.h>	// extension wrangler
// #include <GLFW/glfw3.h> // framework
#include "Tentil.h"
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include "OpenGL.h"

using namespace glm;

int main(int argc, char** argv)
{
	AppWindow wnd(640, 480, argv[0], true);

	Mesh mesh("Resources/meshes/cube.obj", GL_TRIANGLES);
	Shader shader("Resources/shaders/Default.shader");
	
	mat4 proj_mat = perspective(radians(45.0f), 640.0f / 480.0f, 0.1f, 20.0f);
	mat4 view_mat = lookAt(vec3(5.0f, 5.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)); // position, target, orientation
	mat4 model_mat = mat4(1.0f);
	
	Entity E(RenderComponent{ &mesh, &shader });
	
	vec4 test_vec1 = proj_mat * view_mat * model_mat * vec4(1.0f, 1.0f, 1.0f, 1.0f);
	std::cout << "max:" << test_vec1.x << ", " << test_vec1.y << ", " << test_vec1.z << ", " << test_vec1.w << std::endl;
	vec4 test_vec2 = proj_mat * view_mat * model_mat * vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	std::cout << "min:" << test_vec2.x << ", " << test_vec2.y << ", " << test_vec2.z << ", " << test_vec2.w << std::endl;
	
	shader.SetUniformMatrix4f("u_proj", proj_mat);
	shader.SetUniformMatrix4f("u_view", view_mat);
	shader.SetUniformMatrix4f("u_model", model_mat);

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

		entityReg.for_each<RenderComponent>([deltaTime, &model_mat, &shader](const auto entityID, const RenderComponent& RC) {
			model_mat = rotate(model_mat, deltaTime, vec3(-1.0f, 0.5f, 0));
			shader.SetUniformMatrix4f("u_model", model_mat);
			RC.shader->Bind();
			RC.mesh->Draw();
		});

		wnd.Refresh();
	}

	glfwTerminate();
	return 0;
}