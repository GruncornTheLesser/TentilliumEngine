#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <iostream>
#include <list>

#include "Rendering/AppWindow.h"
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/Shader.h"
#include "Rendering/Resources/Texture.h"
#include "Rendering/Resources/GLbuffer.h"

class TestWindow : public AppWindow
{
public:
	// applies only to the current context ie window
	GLuint VAO; // vertex array object - container for buffers and vertex data
	GLuint VBO; // vertex buffer object - all unique vertices
	GLuint IBO; // index buffer object - array of pointers to VBO indices to spell out primitives eg Triangles

	// local to context ie shared
	// big objects eg programs, buffers, textures
	const Shader* shader;
	const Texture* image;

	//local to window
	// "state containers" eg vao, fbo

	TestWindow(const char* imgpath, const char* title) : AppWindow(800, 600, title) {
	
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
		image = Texture::Load(imgpath);
		//auto msh = Mesh::Load();

		shader->setUniform1i("tex", 0);		// tells shader to use texture block 0
		glActiveTexture(GL_TEXTURE0);		// selects which texture unit subsequent texture state calls will affect. 
		image->bind();						// subsequent texture state call. ie adds img texture to unit GL_TEXTURE0

	}
	
	void draw()
	{
		glActiveTexture(GL_TEXTURE0);		// selects which texture unit subsequent texture state calls will affect. 
		image->bind();						// subsequent texture state call. ie adds img texture to unit GL_TEXTURE0

		shader->bind();						// use shader program
		glBindVertexArray(VAO);				// use mesh

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL); // draw mesh in current
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

	auto win1 = new TestWindow("Resources/images/Image1.png", "title 1");
	auto win2 = new TestWindow("Resources/images/Image2.png", "title 2");
	auto win3 = new TestWindow("Resources/images/Image3.png", "title 3");
	auto win4 = new TestWindow("Resources/images/Image1.png", "title 4");
	auto win5 = new TestWindow("Resources/images/Image2.png", "title 5");
	auto win6 = new TestWindow("Resources/images/Image3.png", "title 6");
	
	struct Mat { vec4 a; vec4 b; vec3 c; };
	auto mat1 = new Mat{ vec4(1, 1, 0, 1), vec4(2, 0, 2, 2), vec3(3, 0, 3) };
	auto buf = new GLbuffer(GL_ARRAY_BUFFER, sizeof(Mat), mat1);
	delete mat1;

	auto mat2 = (float*)buf->get<Mat>();
	int i;
	for (i = 0; i < (sizeof(Mat) / sizeof(float)) - 1; i++)
		std::cout << mat2[i] <<", ";
	std::cout << mat2[i] << std::endl;
	delete mat2;
	delete buf;

	AppWindow::Init();
}