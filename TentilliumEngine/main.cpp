#include <GLM/glm.hpp>	// mathematics
#include <GL/glew.h>	// extension wrangler
#include <GLFW/glfw3.h> // framework
#include "Tentil.h"
#include <glm/gtc/matrix_transform.hpp>
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
	
	entityReg.create(RenderComponent{&mesh, &shader});
	
	glm::vec4 test_vec1 = proj_mat * view_mat * model_mat * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	std::cout << "max:" << test_vec1.x << ", " << test_vec1.y << ", " << test_vec1.z << ", " << test_vec1.w << std::endl;
	glm::vec4 test_vec2 = proj_mat * view_mat * model_mat * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	std::cout << "min:" << test_vec2.x << ", " << test_vec2.y << ", " << test_vec2.z << ", " << test_vec2.w << std::endl;
	
	shader.SetUniformMatrix4f("u_proj", proj_mat);
	shader.SetUniformMatrix4f("u_view", view_mat);
	shader.SetUniformMatrix4f("u_model", model_mat);

	// main loop
	float curtime = 0;
	
	while (!wnd.Closed())
	{
		// process new frame
		float glfwTime = glfwGetTime();
		float deltaTime = glfwTime - curtime;
		curtime = glfwTime;
		
		

		/* uniform block example
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), &v);
		*/
		//shader.SetUniform4f("u_color", 0.2f, (sinf(curtime * 2) + 1) / 2, 0.8f, 1.0f);
		//model_mat = glm::translate(model_mat, glm::vec3(0, 0, 1.0f));
		model_mat = glm::rotate(model_mat, glm::radians(0.5f), glm::vec3(-1.0f, 0.5f, 0));
		shader.SetUniformMatrix4f("u_model", model_mat);

		glClear(GL_COLOR_BUFFER_BIT);

		entityReg.for_each<RenderComponent>([](const auto entityID, const RenderComponent& RC) {
			RC.shader->Bind();
			RC.mesh->Draw();
		});

		wnd.Refresh();
	}

	glfwTerminate();
	return 0;
}