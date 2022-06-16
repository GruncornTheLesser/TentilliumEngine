#include "Rendering/Window/AppWindow.h"
#include "Scene.h"
#include "Rendering/Resources/ShaderProgram.h"
#include "Components/Projection.h"
/* TASKS:
*	> entt resource management/c++ embedded resource
*	> Mesh vertex buffer decoupling
*   > 
*/

/*		 
    Y  Z
    | /
	|/ 
	O---X
*/


std::vector<float> positions = {
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
std::vector<float> normals = {
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
std::vector<float> texCoords = {
	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 
	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 
	1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f
};
std::vector<unsigned int> indices = {	// mesh indices
	0, 2, 1, 2, 0, 3, 		// e(0, 2)	 = ---,++-	// back
	4, 5, 6, 6, 7, 4,		// e(4, 6)   =	--+,+++	// front
	8, 9, 10, 10, 4, 8,		// e(8, 10)  = -++,---	// right
	11, 12, 2, 12, 11, 13, 	// e(11, 12) = +++,+--	// left
	10, 14, 5, 5, 4, 10,	// e(5, 10)  = +-+,---	// bottom
	2, 3, 11, 11, 3, 15		// e(3, 11)  = -+-,+++	// top
};

class TestApp : public AppWindow
{
public:
	Scene scene;
	glm::vec2 cam_dir;
	entt::entity root;
	entt::entity camera;
	float time = 0;
	TestApp(const char* title)
		: AppWindow(800, 600, title)
	{
		// create camera
		camera = scene.create();
		scene.emplace<Projection>(camera, glm::radians(60.0f), 800.0f / 600.0f, 0.001f, 100.0f);
		scene.emplace<Transform>(camera, glm::vec3(0, 0, 1));
		scene.set_camera(camera);

		// TODO:
		// load object
		
		//root = scene.load("Resources/meshes/Punk.fbx");
		//scene.get<Transform>(root).position = glm::vec3(0, 0, -1);
	
		root = scene.create();
		scene.emplace<Transform>(root).position = glm::vec3(0,0,-1);

		scene.emplace<VBO<VertAttrib::Index>>(root, indices);
		scene.emplace<VBO<VertAttrib::Position>>(root, positions);
		scene.emplace<VBO<VertAttrib::Normal>>(root, normals);
		scene.emplace<VBO<VertAttrib::TexCoord>>(root, texCoords);

		scene.emplace<SpecularMaterial>(root, glm::vec3(0.5, 0.2, 0.5), 0.0f, 0.0f);

		
		entt::entity copy = scene.create();
		scene.emplace<Transform>(copy, glm::vec3(0,0, -1), glm::vec3(0.1f));
		// copy mesh
		scene.emplace<VBO<VertAttrib::Index>>(copy, scene.get<VBO<VertAttrib::Index>>(root));
		scene.emplace<VBO<VertAttrib::Position>>(copy, scene.get<VBO<VertAttrib::Position>>(root));
		scene.emplace<VBO<VertAttrib::Normal>>(copy, scene.get<VBO<VertAttrib::Normal>>(root));
		// copy material
		scene.emplace<SpecularMaterial>(copy, glm::vec3(0.2, 0.7, 0.2), 0.0f, 0.0f); 
	}

	void onProcess(float delta) {
		scene.get<Transform>(root).position = glm::vec3(cos(time), sin(time += delta), -1);

		auto& cam_trans = scene.get<Transform>(camera);

		if (isPressed(Button::LEFT)) {
			cam_dir.x += (float)m_mouse.getDeltaY() / height;
			cam_dir.y += (float)m_mouse.getDeltaX() / width;
		}
		cam_trans.rotation = glm::quat(glm::vec3(cam_dir, 0));

		glm::vec3 move_direction{ 0, 0, 0 };
		if (isPressed(Key::W)) move_direction.z -= 1;
		if (isPressed(Key::S)) move_direction.z += 1;
		if (isPressed(Key::A)) move_direction.x -= 1;
		if (isPressed(Key::D)) move_direction.x += 1;
		if (isPressed(Key::F)) move_direction.y -= 1;
		if (isPressed(Key::R)) move_direction.y += 1;

		cam_trans.position += cam_trans.rotation * move_direction * delta; // rotate direction by camera rotation
	}

	void onDraw() {
		scene.process();
		refresh();
	}

	void onResize(int width, int height) {
		scene.resize(width, height);
	}

	void onKey(Key key, Action action, Mod mod) { }

	void onMouse(Button button, Action action, Mod mod) { }

	void onMouseMove(int posX, int posY) { }
};

int main(int argc, char** argv)
{
	auto app1 = TestApp("app 1");
	AppWindow::Main({ &app1 });
}