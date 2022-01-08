#include "Tentil.h"
#include <iostream>
#include <gtc/matrix_transform.hpp>

using namespace glm;

int main(int argc, char** argv)
{
	AppWindow* wnd = new AppWindow(640, 480, argv[0], true);
	//Scene*	   scn = new Scene;

	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		
	};

	GLuint VBO;
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	
	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	// recognise vertex attributes eg position, UV, colour, bone weight and bone index
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	auto program = ResourceManager<Shader>::Load("Resources/shaders/Default.shader");
	auto img = ResourceManager<Image>::Load("Resources/images/Image.png");
	// tells shader to use texture block 1
	program->SetUniform1i("tex", 0); 
	// puts texture into texture block 1
	glActiveTexture(GL_TEXTURE0); 
	img->Bind();
	
	program->Bind();


	while (!wnd->Closed())
	{
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		wnd->Refresh();
	}


	/*
	




	auto entity = scn->Create(ModelComponent("Resources/meshes/hat.obj"));
	
	while (!wnd->Closed())
	{
		scn->Render();
		wnd->Refresh();
	}
	*/

	/*
		mat4 proj_mat = perspective(radians(45.0f), 640.0f / 480.0f, 0.1f, 20.0f);
		mat4 view_mat = lookAt(vec3(5.0f, 5.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)); // position, target, orientation
		mat4 model_mat = mat4(1.0f);
		Entity E(
			ModelComponent("Resources/meshes/cube.obj"),
			Shader("Resources/shaders/Default.shader")); // custom shader

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
			entityReg.for_each<ModelComponent, Shader>([](const auto entityID, auto& mesh, auto& shad) {
				shad.Bind();
				mesh.Draw();
			});

			wnd.Refresh();
		}
		*/
}