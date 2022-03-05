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
std::vector<float> vertices = {
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
std::vector<unsigned int> indices = {	// mesh indices
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

	TestWindow(const char* imgpath, const char* title)
		: AppWindow(800, 600, title)
	{
		// load shared resources
		float data[] = { 0, 0, 64, 64, 0, 64, 64, 0, 64, 0, 0, 64, };
		//texture = Texture::stash("hehehe", new Texture(2, 2, 3, data));
		texture = Texture::get(imgpath);

		auto material = std::shared_ptr<Material>(new Material(glm::vec3(1, 2, 3), glm::vec3(4, 5, 5), glm::vec3(4, 3, 2), 1));
		auto mesh = std::shared_ptr<Mesh>(new Mesh(vertices, indices, setupVertex<glm::vec3, glm::vec2>));
		auto e = scene.CreateEntity(Transform(vec3(0, 0, -2)), Model({ mesh }, { material }));

		shader = Shader::get("Resources/shaders/Default.shader");
		shader->setUniform1i("tex", 0);		// tells shader to use texture block 0
			}
	
	~TestWindow() {
		std::cout << "window go bye bye \n";
	}

	void onDraw(float delta)
	{
		glActiveTexture(GL_TEXTURE0);		// selects which texture unit subsequent texture state calls will affect. 
		texture->bind();					// subsequent texture state call. ie adds img texture to unit GL_TEXTURE0

		scene.Testing(glfwGetTime());
		scene.TransformUpdate();
		scene.Render(*shader);
	}
};


int main(int argc, char** argv)
{
	{
		auto win1 = TestWindow("Resources/images/Image4.png", "wnd 1");
		win1.scene.Load("Resources/meshes/crate/glTF/wooden crate.glb");
		auto win2 = TestWindow("texture", "I WANT SOMETHING TO HAPPEN");
		AppWindow::Init({ &win1, &win2 });
	}
}