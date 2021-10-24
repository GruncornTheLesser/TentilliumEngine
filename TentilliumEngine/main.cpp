// #include <GLM/glm.hpp>	// mathematics
// #include <GL/glew.h>	// extension wrangler
// #include <GLFW/glfw3.h> // framework
#include "Tentil.h"
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include "OpenGL.h"

int main(int argc, char** argv)
{
	AppWindow wnd(640, 480, argv[0], true);
	
	Mesh mesh("cube.obj", GL_TRIANGLES);
	Shader shader("Default.shader");

	glm::mat4 proj_mat = glm::perspective(glm::radians(90.0f), 640.0f / 480.0f, 0.1f, 20.0f);
	glm::mat4 view_mat = glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // position, target, orientation
	glm::mat4 model_mat = glm::mat4(1.0f);
	
	Entity E(RenderComponent{ &mesh, &shader });
	
	glm::vec4 test_vec1 = proj_mat * view_mat * model_mat * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	std::cout << "max:" << test_vec1.x << ", " << test_vec1.y << ", " << test_vec1.z << ", " << test_vec1.w << std::endl;
	glm::vec4 test_vec2 = proj_mat * view_mat * model_mat * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
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

		model_mat = glm::rotate(model_mat, deltaTime, glm::vec3(-1.0f, 0.5f, 0));
		shader.SetUniformMatrix4f("u_model", model_mat);

		unsigned int x = GL_MAX_UNIFORM_BUFFER_BINDINGS;

		glClear(GL_COLOR_BUFFER_BIT);

		entityReg.for_each<RenderComponent>([&deltaTime](const auto entityID, const RenderComponent& RC) {
			RC.shader->Bind();
			RC.mesh->Draw();
			deltaTime += 1;
		});

		wnd.Refresh();
	}

	glfwTerminate();
	return 0;
}