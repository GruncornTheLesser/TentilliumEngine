#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <glm.hpp>	// GL maths
#include <iostream>

#include "Scene.h"
#include "Rendering/Window/AppWindow.h"
#include "Rendering/Resources/Model.h"
#include "Rendering/Resources/Shader.h"
#include "Rendering/Resources/Texture.h"
#include "Rendering/Resources/GLbuffer.h"
#include "Components/Transform.h"



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

	entt::entity camera;
	entt::entity root;

	TestWindow(const char* imgpath, const char* title)
		: AppWindow(800, 600, title)
	{
		// create camera
		camera = scene.create();
		scene.emplace<Camera>(camera, glm::radians(75.0f), 800.0f / 600.0f, 0.1, 100);
		scene.emplace<Transform>(camera, glm::vec3(0, 0, 1));
		scene.setCamera(camera);
		
		// load object
		root = scene.load("Resources/meshes/crate/glTF/crate.glb");
		scene.get<Transform>(root).setPosition(0, 0, -1);

		texture = Texture::load("Resources/meshes/animals/texture/wild_animals_map.png");

		shader = Shader::load("Resources/shaders/Default.shader");
		shader->setUniform1i("tex", 0);		// tells shader to use texture block 0
		texture->bind(0);
	}

	void onDraw(float delta)
	{
		scene.Testing(delta, glfwGetTime());
		scene.TransformUpdate();
		scene.Render(*shader);
		refresh();
	}

	void onResize(int width, int height) 
	{
		scene.get<Camera>(scene.getCamera()).resize(width, height);
	}

	void onKey(Key key, bool pressed) 
	{
		AppWindow::onKey(key, pressed);
		std::cout << "KEY: " << (int)key << (pressed ? " pressed" : " released") << std::endl;

		switch (key) {
			case Key::W:
				scene.get<Transform>(scene.getCamera()).deltaPosition(glm::vec3(0, 0, -0.1f));
				break;

			case Key::A:
				scene.get<Transform>(scene.getCamera()).deltaPosition(glm::vec3(-0.1f, 0, 0));
				
				break;

			case Key::S: 
				scene.get<Transform>(scene.getCamera()).deltaPosition(glm::vec3(0, 0, 0.1f));
				break;

			case Key::D:
				scene.get<Transform>(scene.getCamera()).deltaPosition(glm::vec3(0.1f, 0, 0));
				break;

			case Key::R:
				scene.get<Transform>(scene.getCamera()).deltaPosition(glm::vec3(0, 0.1f, 0));
				break;

			case Key::F:
				scene.get<Transform>(scene.getCamera()).deltaPosition(glm::vec3(0, -0.1f, 0));
				break;

			case Key::Q:
				scene.get<Transform>(scene.getCamera()).deltaRotation(glm::vec3(0, 0.05f, 0));
				break;

			case Key::E:
				scene.get<Transform>(scene.getCamera()).deltaRotation(glm::vec3(0, -0.05f, 0));
				break;
		}
	}

	void onMouse(Button button, int pressed) 
	{
		AppWindow::onMouse(button, pressed);
		if (button == Button::SCROLL) std::cout << "SCROLL: " << pressed << std::endl; // scroll is the offset
		else std::cout << "BUTTON: " << (int)button << (pressed ? " pressed" : " released") << std::endl;	
	}

	void onMouseMove(int posX, int posY, int deltaX, int deltaY) 
	{
		AppWindow::onMouseMove(posX, posY, deltaX, deltaY);
	}
};

int main(int argc, char** argv)
{
	auto win1 = TestWindow("Resources/images/Image4.png", "wnd 1");
	AppWindow::Main({ &win1 });
	
}