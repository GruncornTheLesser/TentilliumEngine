#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <iostream>

#include "Rendering/AppWindow.h"
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/Shader.h"
#include "Rendering/Resources/Texture.h"
#include "Rendering/Resources/GLbuffer.h"

using namespace glm;

class TestWindow : public AppWindow
{
public:
	// applies only to the current context ie window
	GLuint VAO; // vertex array object - container for buffers and vertex data
	GLuint VBO; // vertex buffer object - all unique vertices
	GLuint IBO; // index buffer object - array of pointers to VBO indices to spell out primitives eg Triangles

	// shared between windows (local to context which is currently shared between all windows) 
	// big objects eg programs, buffers, textures
	const Shader* shader;
	const Texture* image;

	glm::mat4 transform;
	glm::mat4 perspective;
	//local to window
	// "state containers" eg vao, fbo

	TestWindow(const char* imgpath, const char* title) : AppWindow(800, 600, title) {
		// vertex data
		float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 0
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 1
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 2
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 3
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 4
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 5
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 6
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 7
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 8
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 9
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 10
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 11
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 12
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 13
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // 14
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // 15
		};
		unsigned int indices[] = {	// mesh indices
			0, 2, 1, 2, 0, 3, 		// e(0, 2)	 = ---,++-	// back
			4, 5, 6, 6, 7, 4,		// e(4, 6)   =	--+,+++	// front
			8, 9, 10, 10, 4, 8,		// e(8, 10)  = -++,---	// right
			11, 12, 2, 12, 11, 13, 	// e(11, 12) = +++,+--	// left
			10, 14, 5, 5, 4, 10,	// e(5, 10)  = +-+,---	// bottom
			2, 3, 11, 11, 3, 15		// e(3, 11)  = -+-,+++	// top
		}; 


		// generate buffers
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);
		
		// bind buffers
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		
		// set buffer data -> 
		// buffer target - (GL_ARRAY, GL_UNIFORM_BUFFER, GL_ELEMENT_ARRAY_BUFFER...),
		// size - the size of the buffer, 
		// ptr - a pointer to the data, 
		// hint - general use of the buffer (GL_STATIC, GL_STREAM_DRAW, GL_DYNAMIC, GL_DRAW, GL_READ, GL_COPY)
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// define vertex attributes -> 
		// index - attribute no, 
		// size - size in number of type(1,2,3,4), 
		// type - the type of the attributes (GL_BYTE, GL_SHORT, GL_FLOAT, GL_INT...), 
		// stride - the size of the vertex, 
		// pointer - offset from the start of the vertex
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// unbind buffers
		glBindVertexArray(NULL);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

		// load resources from file
		shader = Shader::Get("Resources/shaders/Default.shader");
		image = Texture::Get(imgpath);
		//auto msh = Mesh::Load();

		shader->setUniform1i("tex", 0);		// tells shader to use texture block 0
		glActiveTexture(GL_TEXTURE0);		// selects which texture unit subsequent texture state calls will affect. 
		image->bind();						// subsequent texture state call. ie adds img texture to unit GL_TEXTURE0

		perspective = glm::perspective(2.0f, 4.0f/3.0f, 0.001f, 100.0f);

	}
	
	void onDraw(float delta)
	{
		glActiveTexture(GL_TEXTURE0);		// selects which texture unit subsequent texture state calls will affect. 
		image->bind();						// subsequent texture state call. ie adds img texture to unit GL_TEXTURE0

		shader->bind();						// use shader program
		glBindVertexArray(VAO);				// use mesh

		transform = glm::translate(mat4(1.0f), vec3(0, 0, -1));
		transform = glm::scale(transform, vec3(0.25f));
		transform *= (mat4)glm::quat(vec3(0.5f * sin(0.3f * glfwGetTime()), glfwGetTime(), 0));
		
		transform = perspective * transform;
		shader->setUniformMatrix4f("MVP", transform);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL); // draw mesh in current
	
		//setTitle(std::to_string(delta));
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

	auto win1 = new TestWindow("Resources/images/Image4.png", "wnd 1");
	auto win2 = new TestWindow("Resources/images/Image1.png", "wnd 2");
	/*
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
	*/
	AppWindow::Init();
}