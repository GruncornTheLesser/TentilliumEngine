#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <iostream>
#include <list>

#include "Rendering/AppWindow.h"
#include "Rendering/Resources/Material.h"
#include "Rendering/UniformBuffer.h"

class TestWindow : public AppWindow
{
public:
	// applies only to the current context ie window
	GLuint VAO; // vertex array object - container for buffers and vertex data
	GLuint VBO; // vertex buffer object - all unique vertices
	GLuint IBO; // index buffer object - array of pointers to VBO indices to spell out primitives eg Triangles

	const Shader* shader;
	const Image* image;

	TestWindow(const char* imgpath, const char* title) : AppWindow(800, 600, title) {
		makeCurrent();
		
		// vertex data
		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		};
		unsigned int indices[] = { 0, 1, 2, 2, 1, 3 }; // mesh indices


		// generate buffers
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);
		// bind buffers
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		// set buffer data
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// recognise vertex attributes eg position, UV, colour, bone weight and bone index
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// unbind buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// load resources from file
		shader = Shader::Load("Resources/shaders/Default.shader");
		image = Image::Load(imgpath);
		//auto msh = Mesh::Load();



		shader->setUniform1i("tex", 0);		// tells shader to use texture block 0
		glActiveTexture(GL_TEXTURE0);		// selects which texture unit subsequent texture state calls will affect. 
		image->bind();						// subsequent texture state call. ie adds img texture to unit GL_TEXTURE0
		
		shader->bind();						// use shader program
		glBindVertexArray(VAO);				// use mesh

		glfwMakeContextCurrent(NULL);


	}

	void Draw()
	{
		shader->bind();						// use shader program
		glBindVertexArray(VAO);				// use mesh

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL); // draw mesh in current
		refresh();
	}
};

int main(int argc, char** argv)
{
	/*
	Scene scn;

	auto e0 = scn.CreateEntity();
	auto e1 = scn.CreateEntity();
	auto e2 = scn.CreateEntity();
	auto e3 = scn.CreateEntity();
	auto e4 = scn.CreateEntity();
	auto e5 = scn.CreateEntity();
	auto e6 = scn.CreateEntity();
	auto e7 = scn.CreateEntity();

	scn.AddComponents(e7, Hierarchy(e4), Transform(vec3(19, 1, 7)));
	scn.AddComponents(e6, Hierarchy(e4), Transform(vec3(17, 1, 6)));
	scn.AddComponents(e5, Hierarchy(e4), Transform(vec3(13, 1, 5)));
	scn.AddComponents(e4, Hierarchy(e1), Transform(vec3(11, 1, 4)));
	scn.AddComponents(e3, Hierarchy(e0), Transform(vec3(7,  1, 3)));
	scn.AddComponents(e2, Hierarchy(e0), Transform(vec3(5,  1, 2)));
	scn.AddComponents(e1, Hierarchy(e0), Transform(vec3(3,  1, 1)));
	scn.AddComponents(e0,				 Transform(vec3(2,  1, 0)));

	//scn.TagUpdate();
	//scn.Calculate();
	*/

	std::list<TestWindow*> windows;
	windows.push_back(new TestWindow("Resources/images/Image3.png", "title 1"));
	windows.front()->makeCurrent();
	
	auto mat = Material(vec4(1, 1, 0, 1), vec4(2, 0, 2, 2), vec3(1, 2 ,3));

	auto arr = mat.getBufferData();
	delete[] arr;

	while (windows.size() > 0)
	{
		windows.remove_if([](TestWindow* wnd) 
		{ 
			wnd->Draw();

			if (wnd->Closed()) 
			{
				delete wnd;
				return true;
			}
			else
				return false;
		});
	}

	glfwTerminate();
}