#include "Rendering/Window/AppWindow.h"
#include "Scene.h"
#include "Rendering/Resources/ShaderProgram.h"
#include "Components/Projection.h"
#include "Components/PointLight.h"

/* REVIEW:
*	> localize reference counting to Resource
*	> replace current flag system with replace and a entt tag
*	> Components made so far to improve syntax
*	> Tidy Shader functionality into something less ugly
*/

/* TODO:
*	> Transparent objects
*		> Tag transparent objects on material initiation
*	
*	> PointLight Buffer Pagination
*	> Material Buffer Pagination
* 
*	> Light updates on position, scale and (rotation + projection) association as well as directional + spotlights
*	> Normal + Tangent VBO generation
* 
*	> Tidy AppWindow functionality into something less ugly
*		> rethink input system
* 
* 
*   > bones, skinning and animation
*/

/* RESEARCH:
*	> Morton encoding for position in a quad tree -> chunking???
*/

/*
	Y  Z
	| /
	|/
	O---X
*/

// PointLight only stores Colour, 
// without Position its a directional light
// with Position its a pointlight (and/or Scale)
// with Position and Projection its a Spotlight (and/or Scale)
// Scale determines Spotlight distance and Pointlight radius

// textures
unsigned char Rtest[] = {
	0xff, 
	0x7f,
	0x3f, 
	0x1f, 
};
unsigned char RGtest[] = {
	0xff, 0x00,
	0x00, 0xff,
	0x00, 0xff,
	0xff, 0x00,
};
unsigned char RGBtest[] = {
	0xff, 0x00, 0xff, 
	0x00, 0x00, 0xff, 
	0x00, 0x00, 0xff, 
	0xff, 0x00, 0xff, 
};
unsigned char RGBAtest[] = {
	0xff, 0x00, 0xff, 0xff,
	0x00, 0x00, 0xff, 0xff,
	0x00, 0x00, 0xff, 0xff,
	0xff, 0x00, 0xff, 0xff,
};

using namespace Transform;
using namespace Mesh;

class TestApp : public AppWindow
{
public:
	Scene scene;
	glm::vec2 cam_dir;

	entt::entity obj;
	entt::entity box1;
	entt::entity box2 = entt::tombstone;
	entt::entity root;

	float time;

	std::vector<entt::entity> lights;

	TestApp(const char* title) : AppWindow(800, 600, title)
	{
		scene.size = AppWindow::size;

		{
			entt::entity cam = scene.create();
			scene.emplace<Projection>(cam, glm::radians(60.0f), 800.0f / 600.0f, 0.02f, 50.0f);
			scene.emplace<Position>(cam, 0, 1, 0);
			scene.emplace<Rotation>(cam);
			scene.camera = cam;
		}

		{
			obj = scene.load("Resources/meshes/Sponza.fbx");
			scene.emplace<Position>(obj, 0, 0, -1);
			scene.emplace<Rotation>(obj);
		}

		for (int x = 0; x < 20; x++) {
			for (int y = 0; y < 20; y++) {
				for (int z = 0; z < 5; z++) {
					entt::entity light = scene.create();
					scene.emplace<PointLight>(light,
						glm::vec3((rand() % 255 / 255.0f * 2 - 1) * 10, z * 2 + 1, (rand() % 255 / 255.0f * 2 - 1) * 10),
						glm::vec3(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f), 3.0f);
				}
			}
		}
	}

	void onProcess(float delta) {
		time += delta;

		glm::vec3 camera_position = scene.get<Position>(scene.camera);
		glm::quat camera_rotation = scene.get<Rotation>(scene.camera);
		
		if (isPressed(Button::LEFT)) {
			const float sensitivity = 2;
			cam_dir += (glm::vec2)m_mouse.getDelta() / glm::vec2(getSize()) * sensitivity;
			
			//FPS camera:  RotationX(pitch) * RotationY(yaw)
			glm::quat pitch = glm::angleAxis(cam_dir.y, glm::vec3(1, 0, 0));
			glm::quat yaw = glm::angleAxis(cam_dir.x, glm::vec3(0, 1, 0));
			
			camera_rotation = glm::normalize(yaw * pitch);

			scene.replace<Rotation>(scene.camera, camera_rotation);
		}

		glm::vec3 move_direction;
		if (isPressed(Key::W)) move_direction.z -= 1;
		if (isPressed(Key::S)) move_direction.z += 1;
		if (isPressed(Key::A)) move_direction.x -= 1;
		if (isPressed(Key::D)) move_direction.x += 1;
		if (isPressed(Key::F)) move_direction.y -= 1;
		if (isPressed(Key::R)) move_direction.y += 1;

		if (isPressed(Key::LEFT_SHIFT))		move_direction *= 10;
		if (isPressed(Key::LEFT_CONTROL))	move_direction *= 0.1;


		if (move_direction != glm::vec3(0, 0, 0))
			scene.replace<Position>(scene.camera, camera_rotation * move_direction * delta + camera_position); // rotate direction by camera rotation
		

		//scene.replace<Position>(box1, cos(time), sin(time), 0.0f);
		//scene.set<Position>(box2, -sin(time), -cos(time), 0.0f);
		//scene.replace<Rotation>(obj, 0.0, time, 0);
	}

	void onDraw() {
		scene.process();
		refresh();
	}

	void onResize(glm::ivec2 size) {
		scene.setSize(size);
	}

	void onKey(Key key, Action action, Mod mod) { }

	void onMouse(Button button, Action action, Mod mod) { }

	void onMouseMove(glm::ivec2 position) { }
};

int main(int argc, char** argv)
{
	// when the program is closed from the X in the console there are memory leaks
	auto app1 = TestApp("app 1");
	AppWindow::main({ &app1 });
}