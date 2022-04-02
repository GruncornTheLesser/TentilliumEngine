#include "Rendering/Window/AppWindow.h"
#include "Scene/Scene.h"
#include <iostream>

class TestApp : public AppWindow
{
public:
	Scene scene;
	glm::vec3 cam_dir;
	entt::entity root;
	ShaderProgram program{ "Resources/shaders/default.shader" };

	TestApp(const char* title)
		: AppWindow(800, 600, title)
	{
		// create camera
		scene.camera = scene.create();
		scene.emplace<Camera>(scene.camera, glm::radians(75.0f), 800.0f / 600.0f, 0.001f, 100.0f);
		scene.emplace<Transform>(scene.camera, glm::vec3(0, 0, 1));
		
		// load object
		root = scene.load("Resources/meshes/Person.fbx");
		scene.get<Transform>(root).position = glm::vec3(0, 0, -1);
	}

	void onProcess(float delta) {
		auto& cam_trans = scene.get<Transform>(scene.camera);

		if (isPressed(Button::LEFT)) {
			cam_dir.x += (float)m_mouse.getDeltaY() / height;
			cam_dir.y += (float)m_mouse.getDeltaX() / width;
		}
		cam_trans.rotation = glm::quat(cam_dir);

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
		scene.SceneGraphUpdate(); 
		scene.Render(program);
		refresh();
	}

	void onResize(int width, int height) {
		scene.get<Camera>(scene.camera).resize(width, height);
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