#include "Tentil.h"
#include "Components/Scene.h"
#include <iostream>
#include <queue>
#include <list>
#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

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
/*
	auto e0 = reg.create();
	auto e1 = reg.create();
	auto e2 = reg.create();
	auto e3 = reg.create();
	auto e4 = reg.create();
	auto e5 = reg.create();
	auto e6 = reg.create();
	auto e7 = reg.create();

	// components must be initiated such that the components parent is before the child
	// however because the entt storage iterates backwards it means that the component
	// order is reversed. bottom line:
	// components parents must be sorted after their children or the system will break
	auto scn = Scene::GetInstance();
	scn->Add(e7, Hierarchy(e4), Transform(vec3(8, 1, 2)));
	scn->Add(e6, Hierarchy(e4), Transform(vec3(7, 1, 2)));
	scn->Add(e5, Hierarchy(e4), Transform(vec3(6, 1, 2)));
	scn->Add(e4, Hierarchy(e1), Transform(vec3(5, 1, 2)));
	scn->Add(e3, Hierarchy(e0), Transform(vec3(4, 1, 1)));
	scn->Add(e2, Hierarchy(e0), Transform(vec3(3, 1, 1)));
	scn->Add(e1, Hierarchy(e0), Transform(vec3(2, 1, 1)));
	scn->Add(e0, Transform(vec3(1, 1, 0)));

	scn->tick();

*/

int main(int argc, char** argv)
{

	glm::mat4 mat = mat4(1.0);

	vec3 pos = vec3(2, 12, 4);
	vec3 r = vec3(radians(100.0f), radians(20.0f), radians(120.0f));
	quat rot = quat(r);
	vec3 sca = vec3(2, 1, 9);
	
	

	mat = translate(mat4(1.0f), pos);
	mat = scale(mat, sca);
	mat *= mat4(rot);
	


	std::cout << "pos : " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	std::cout << "scl : " << sca.x << ", " << sca.y << ", " << sca.z << std::endl;
	std::cout << "rot : " << rot.x << ", " << rot.y << ", " << rot.z << ", " << rot.w << std::endl;
	std::cout << "r   : " << degrees(r.x) << ", " << degrees(r.y) << ", " << degrees(r.z) << std::endl;
	
	pos = vec3();
	r = vec3();
	rot = quat();
	sca = vec3();

	std::cout << std::endl;

	Transform::ExtractRotation(mat, pos, sca, rot);
	r = eulerAngles(rot);

	std::cout << "pos : " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	std::cout << "scl : " << sca.x << ", " << sca.y << ", " << sca.z << std::endl;
	std::cout << "rot : " << rot.x << ", " << rot.y << ", " << rot.z << ", " << rot.w << std::endl;
	std::cout << "r   : " << degrees(r.x) << ", " << degrees(r.y) << ", " << degrees(r.z) << std::endl;

	std::cout << std::endl;
	

	// vertex data
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
	};
	unsigned int indices[] = { 0, 1, 2, 2, 1, 3 }; // mesh indices


	std::list<TestWindow*> windows;
	windows.push_back(new TestWindow("Resources/images/Image1.png", "title 1"));
	windows.push_back(new TestWindow("Resources/images/Image2.png", "title 2"));//new AppWindow(640, 480, "2", true);//
	windows.push_back(new TestWindow("Resources/images/Image3.png", "title 3"));

	while (windows.size() > 0)
	{
		windows.remove_if([](TestWindow* wnd) 
		{ 
			wnd->makeCurrent();
			wnd->Draw();

			if (wnd->Closed()) 
				delete wnd;
				
			return wnd->Closed();
		});
	}

	glfwTerminate(); // technically unnecessary
}