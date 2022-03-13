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
#include <stdint.h>


using namespace glm;

/*
std::vector<float> vertices = {
	-0.5f, -0.5f, -0.5f, 
	 0.5f, -0.5f, -0.5f,  
	 0.5f,  0.5f, -0.5f,  
	-0.5f,  0.5f, -0.5f,  
	-0.5f, -0.5f,  0.5f,  
	 0.5f, -0.5f,  0.5f,  
	 0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f, -0.5f,  
	-0.5f, -0.5f, -0.5f,  
	 0.5f,  0.5f,  0.5f,  
	 0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f,  0.5f,  
	 0.5f, -0.5f, -0.5f,  
	-0.5f,  0.5f,  0.5f  
};
std::vector<float> tCoords = {
	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f
};
std::vector<unsigned int> indices = {	// mesh indices
	0, 2, 1, 2, 0, 3, 		// e(0, 2)	 = ---,++-	// back
	4, 5, 6, 6, 7, 4,		// e(4, 6)   =	--+,+++	// front
	8, 9, 10, 10, 4, 8,		// e(8, 10)  = -++,---	// right
	11, 12, 2, 12, 11, 13, 	// e(11, 12) = +++,+--	// left
	10, 14, 5, 5, 4, 10,	// e(5, 10)  = +-+,---	// bottom
	2, 3, 11, 11, 3, 15		// e(3, 11)  = -+-,+++	// top
};
*/

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
		//float temp[12] = { 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1 }; // this is the data in the image
		//texture = Texture::stash(imgpath, new Texture(2, 2, 3, temp));
		//texture = Texture::load(imgpath);
		//scene.HierarchyUpdate();


		//auto material = std::shared_ptr<Material>(new Material());
		//auto mesh = std::shared_ptr<Mesh>(new Mesh(material, indices, vertices, &tCoords));
		//auto e = scene.create();
		//scene.emplace<Transform>(e, glm::vec3(0, 0, -2));
		//scene.emplace<Model>(e, std::vector<std::shared_ptr<Mesh>>{ mesh });
		//texture = Texture::load(imgpath);

		scene.load("Resources/meshes/animals/fbx/deer_1.fbx");
		//scene.load("Resources/meshes/animals/fbx/fox.fbx");
		//scene.load("Resources/meshes/animals/fbx/bear.fbx");
		texture = Texture::load("Resources/meshes/animals/texture/wild_animals_map.png");


		//float temp[12] = { 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1 }; // this is the data in the image
		texture = Texture::load("Resources/meshes/animals/texture/wild_animals_map.png");

		shader = Shader::load("Resources/shaders/Default.shader");
		shader->setUniform1i("tex", 0);		// tells shader to use texture block 0
	}

	void onDraw(float delta)
	{
		glActiveTexture(GL_TEXTURE0);		// selects which texture unit subsequent texture state calls will affect. 
		texture->bind();					// subsequent texture state call. ie adds img texture to unit GL_TEXTURE0

		scene.Testing(glfwGetTime());
		scene.TransformUpdate();
		scene.Render(*shader);

		refresh();
	}
};

int main(int argc, char** argv)
{
	auto win1 = new TestWindow("Resources/images/Image4.png", "wnd 1");
	AppWindow::Init({ win1 });
}