#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include "Scene.h"
#include "Rendering/AppWindow.h"
#include "Rendering/Resources/Model.h"
#include "Rendering/Resources/Shader.h"
#include "Rendering/Resources/Texture.h"
#include "Rendering/Resources/GLbuffer.h"
#include "Components/Transform.h"

using namespace glm;

const float vertices[80] = {
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
const unsigned int indices[36] = {	// mesh indices
	0, 2, 1, 2, 0, 3, 		// e(0, 2)	 = ---,++-	// back
	4, 5, 6, 6, 7, 4,		// e(4, 6)   =	--+,+++	// front
	8, 9, 10, 10, 4, 8,		// e(8, 10)  = -++,---	// right
	11, 12, 2, 12, 11, 13, 	// e(11, 12) = +++,+--	// left
	10, 14, 5, 5, 4, 10,	// e(5, 10)  = +-+,---	// bottom
	2, 3, 11, 11, 3, 15		// e(3, 11)  = -+-,+++	// top
};

class TestWindow : public AppWindow
{
public:
	// shared between windows (local to context which is currently shared between all windows) 
	// big objects eg programs, buffers, textures

	Scene scene;

	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Mesh> mesh; // meshes are local to the window
	std::shared_ptr<Material> material;
	Model model;

	Transform transform;
	glm::mat4 perspective;

	unsigned int VAO, VBO, IBO;

	TestWindow(const char* imgpath, const char* title)
		: AppWindow(800, 600, title)
	{
		// load resources
		shader = Shader::Get("Resources/shaders/Default.shader");
		texture = Texture::Get(imgpath);

		// load function hasnt been finished
		material = std::shared_ptr<Material>(new Material(glm::vec3(1, 2, 3), glm::vec3(4, 5, 5), glm::vec3(4, 3, 2), 1));
		// .mtl, gltf


		mesh = std::shared_ptr<Mesh>(new Mesh(vertices, sizeof(vertices), indices, sizeof(indices), setupVertex<glm::vec3, glm::vec2>));

		// local to window
		model = Model({ mesh }, { material });

		shader->setUniform1i("tex", 0);		// tells shader to use texture block 0
		glActiveTexture(GL_TEXTURE0);		// selects which texture unit subsequent texture state calls will affect. 
		texture->bind();					// subsequent texture state call. ie adds img texture to unit GL_TEXTURE0

		perspective = glm::perspective(2.0f, 4.0f/3.0f, 0.001f, 100.0f);
	}
	
	void onDraw(float delta)
	{
		glActiveTexture(GL_TEXTURE0);		// selects which texture unit subsequent texture state calls will affect. 
		texture->bind();					// subsequent texture state call. ie adds img texture to unit GL_TEXTURE0

		// update transform
		transform.setPosition(vec3(0, 0, -5));
		transform.setRotation(quat(vec3(0.5f * sin(0.3f * glfwGetTime()), glfwGetTime(), 0)));
		transform.UpdateLocal();
		transform.UpdateWorld();

		// adjust to camera perspective
		mat4 mvp = perspective * (mat4)transform;
		shader->setUniformMatrix4f("MVP", mvp);	// set uniform

		shader->bind();							// use shader program
		model.draw();
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
	//auto win2 = new TestWindow("Resources/images/Image1.png", "wnd 2");
	
	AppWindow::Init();
}