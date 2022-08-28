#include "Rendering/Window/AppWindow.h"
#include "Scene.h"
#include "Rendering/Resources/ShaderProgram.h"
#include "Components/Projection.h"

/* RESEARCH:
*	> Morton encoding for position in a quad tree -> chunking???
*/

/* TASKS:
*   > forward+ render pipeline	
*		-> depth pre pass
*		-> light culling
*		-> light accumulation
* 
*   > bones, skinning and animation
* 
*	> seperate Transform and Transform implementation
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
	entt::entity obj;
	entt::entity box1;
	entt::entity box2;
	entt::entity root;

	float time;

	TestApp(const char* title)
		: AppWindow(800, 600, title)
	{
		root = scene.create();									// 0
		box1 = scene.create();									// 1
		box2 = scene.create();									// 2
		obj = scene.load("Resources/meshes/Person.fbx");		// 3
		scene.camera = scene.create();							// 4
		
		// create obj
		scene.emplace<Hierarchy>(obj, root);
		scene.emplace<Transform>(obj);

		// create box1
		scene.emplace<Hierarchy>(box1, root);
		scene.emplace<Transform>(box1, glm::vec3(-0.5, 0, -1), glm::vec3(0.5f));
		scene.emplace<VBO<Index>>(box1, indices);
		scene.emplace<VBO<Position>>(box1, positions);
		scene.emplace<VBO<Normal>>(box1, normals);
		scene.emplace<VBO<TexCoord>>(box1, texCoords);
		scene.emplace<SpecularMaterial>(box1, glm::vec3(0.2, 0.7, 0.2), 0.0f, 0.0f);

		// create box2
		scene.emplace<Hierarchy>(box2, box1);
		scene.emplace<Transform>(box2, glm::vec3(0.5, 0, -1), glm::vec3(0.5f));
		scene.emplace<VBO<Index>>(box2, scene.get<VBO<Index>>(box1));
		scene.emplace<VBO<Position>>(box2, scene.get<VBO<Position>>(box1));
		scene.emplace<VBO<Normal>>(box2, scene.get<VBO<Normal>>(box1));
		scene.emplace<VBO<TexCoord>>(box2, scene.get<VBO<TexCoord>>(box1));
		scene.emplace<SpecularMaterial>(box2, glm::vec3(0.2, 0.7, 0.7), 0.0f, 0.0f);

		// create camera
		scene.emplace<Projection>(camera, glm::radians(60.0f), 800.0f / 600.0f, 0.001f, 100.0f);
		scene.emplace<Transform>(camera, glm::vec3(0, 0, 1));
		scene.set_camera(camera);

	}

	void onProcess(float delta) {
		
		title = std::to_string(1 / delta).c_str();

		if (scene.valid(box1)) scene.get<Transform>(box1).position = glm::vec3(cos(time), sin(time), -1);
		if (scene.valid(box2)) scene.get<Transform>(box2).position = glm::vec3(-sin(time), -cos(time), -1);

		time += delta;

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
	
		if (isPressed(Key::LEFT_SHIFT)) {
			if (scene.valid(box1)) scene.get_or_emplace<Hierarchy>(obj, box1).parent = box1;
		}
		if (isPressed(Key::LEFT_CONTROL)) {
			if (scene.valid(box2)) scene.get_or_emplace<Hierarchy>(obj, box2).parent = box2;
		}
		if (isPressed(Key::Z)) {
			if (scene.valid(box2)) scene.destroy(box2);
		}
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
	// when the program is closed from the X in the console there are memory leaks
	auto app1 = TestApp("app 1");
	AppWindow::Main({ &app1 });
}