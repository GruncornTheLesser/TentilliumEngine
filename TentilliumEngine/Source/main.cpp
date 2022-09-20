#include "Rendering/Window/AppWindow.h"
#include "Scene.h"
#include "Rendering/Resources/ShaderProgram.h"
#include "Components/Projection.h"
#include "Components/PointLight.h"

/* FINALIZE:
*
*/

/* REVIEW:
*	> localize reference counting to Resource
*	> replace current flag system with replace and a entt tag
*	> Components made so far to improve syntax
*	> Tidy Shader functionality into something less ugly
*/

/* TODO:
*	> Rendering
*	> seperate rendering functionality further for mesh/camera/rendering type
* 
*	> Tidy AppWindow functionality into something less ugly
*		> rethink input system
*	
*   > bones, skinning and animation
*	> seperate Transform and Transform implementation
*/

/* RESEARCH:
*	> Morton encoding for position in a quad tree -> chunking???
*	> ANGLE a library that compiles opengl calls into directx
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

class TestApp : public AppWindow
{
public:
	Scene scene;
	glm::vec2 cam_dir;

	entt::entity obj;
	entt::entity box1;
	entt::entity box2;
	entt::entity root;
	entt::entity light1, light2;

	float time;

	std::vector<entt::entity> lights;

	TestApp(const char* title) : AppWindow(800, 600, title)
	{
		scene.size = AppWindow::size;

		root = scene.create();

		// create camera
		{ 
			entt::entity cam = scene.create();
			scene.set<Projection>(cam, glm::radians(60.0f), 800.0f / 600.0f, 0.02f, 100.0f);
			scene.set<Position>(cam, 0, 1, 0);
			scene.set<Rotation>(cam);
			scene.camera = cam;
		}

		// create obj
		{
			obj = scene.load("Resources/meshes/Person.fbx");
			scene.set<Parent>(obj, root);
			scene.set<Position>(obj, 0, 0, -2);
			scene.set<Rotation>(obj);
		}

		// create box1
		{
			box1 = scene.create();
			scene.set<Parent>(box1, obj);
			
			scene.set<Position>(box1, -0.5, 0, 0);
			scene.set<Scale>(box1, 0.5f);
			
			scene.set<VBO<V_Index>>(box1, indices);
			scene.set<VBO<V_Position>>(box1, positions);
			scene.set<VBO<V_Normal>>(box1, normals);
			scene.set<VBO<V_TexCoord>>(box1, texCoords);
			
			scene.set<Material>(box1, Texture("Resources/textures/pigeon.jpg"));
		}

		// create box2
		{
			box2 = scene.create();
			scene.set<Parent>(box2, box1);
			
			scene.set<Position>(box2, 0.5, 0, 0);
			scene.set<Scale>(box2, 0.5f);
			
			scene.set<VBO<V_Index>>(box2, scene.get<VBO<V_Index>>(box1));
			scene.set<VBO<V_Position>>(box2, scene.get<VBO<V_Position>>(box1));
			scene.set<VBO<V_Normal>>(box2, scene.get<VBO<V_Normal>>(box1));
			scene.set<VBO<V_TexCoord>>(box2, scene.get<VBO<V_TexCoord>>(box1));
			
			scene.set<Material>(box2, Texture(RGBtest, 2, 2, Texture::Format::RGB));
		}
		
		// floor
		{
			entt::entity floor = scene.create();
			
			scene.set<Position>(floor, 0, -0.1f, 0);
			scene.set<Scale>(floor, 1000, 0, 1000);
			
			scene.set<VBO<V_Index>>(floor, scene.get<VBO<V_Index>>(box1));
			scene.set<VBO<V_Position>>(floor, scene.get<VBO<V_Position>>(box1));
			scene.set<VBO<V_Normal>>(floor, scene.get<VBO<V_Normal>>(box1));
			scene.set<VBO<V_TexCoord>>(floor, scene.get<VBO<V_TexCoord>>(box1));
			
			scene.set<Material>(floor, Texture("Resources/textures/grid.png"));
		}

		// light 1
		{
			light1 = scene.create();
			scene.set<PointLight>(light1, glm::vec3(0.5f, 1, 0), glm::vec3(1, 0, 0), 2.0f);
			
			scene.set<Position>(light1, 0.5f, 1, 0);
			scene.set<Scale>(light1, 0.1f);
			
			scene.set<VBO<V_Index>>(light1, scene.get<VBO<V_Index>>(box1));
			scene.set<VBO<V_Position>>(light1, scene.get<VBO<V_Position>>(box1));
			scene.set<VBO<V_Normal>>(light1, scene.get<VBO<V_Normal>>(box1));
			scene.set<VBO<V_TexCoord>>(light1, scene.get<VBO<V_TexCoord>>(box1));
			
			scene.set<Material>(light1, glm::vec4(1, 0, 0, 1));
		}

		// light 2
		{
			light2 = scene.create();
			scene.set<PointLight>(light2, glm::vec3(-0.5f, 1, 0), glm::vec3(0, 0, 1), 2.0f);
			
			scene.set<Position>(light2, -0.5f, 1, 0);
			scene.set<Scale>(light2, 0.1f);

			scene.set<VBO<V_Index>>(light2, scene.get<VBO<V_Index>>(box1));
			scene.set<VBO<V_Position>>(light2, scene.get<VBO<V_Position>>(box1));
			scene.set<VBO<V_Normal>>(light2, scene.get<VBO<V_Normal>>(box1));
			scene.set<VBO<V_TexCoord>>(light2, scene.get<VBO<V_TexCoord>>(box1));
			
			scene.set<Material>(light2, glm::vec4(0, 0, 1, 1));
		}
	}

	void onProcess(float delta) {
		time += delta;

		if (isPressed(Button::LEFT)) cam_dir;
		
		glm::vec3 move_direction;
		if (isPressed(Key::W)) move_direction.z -= 1;
		if (isPressed(Key::S)) move_direction.z += 1;
		if (isPressed(Key::A)) move_direction.x -= 1;
		if (isPressed(Key::D)) move_direction.x += 1;
		if (isPressed(Key::F)) move_direction.y -= 1;
		if (isPressed(Key::R)) move_direction.y += 1;

		if (isPressed(Key::LEFT_SHIFT))		move_direction *= 10;
		if (isPressed(Key::LEFT_CONTROL))	move_direction *= 0.1;
		
		glm::vec3 camera_position = scene.get<Position>(scene.camera);
		glm::quat camera_rotation = scene.get<Rotation>(scene.camera);
		
		if (isPressed(Button::LEFT)) {
			const float sensitivity = 2;
			cam_dir += (glm::vec2)m_mouse.getDelta() / glm::vec2(getSize()) * sensitivity;
			
			//FPS camera:  RotationX(pitch) * RotationY(yaw)
			glm::quat pitch = glm::angleAxis(cam_dir.y, glm::vec3(1, 0, 0));
			glm::quat yaw = glm::angleAxis(cam_dir.x, glm::vec3(0, 1, 0));
			
			camera_rotation = glm::normalize(yaw * pitch);
		}

		scene.set<Rotation>(scene.camera, camera_rotation);
		scene.set<Position>(scene.camera, camera_rotation * move_direction * delta + camera_position); // rotate direction by camera rotation
		

		scene.set<Position>(box1, cos(time), sin(time), 0.0f);
		scene.set<Position>(box2, -sin(time), -cos(time), 0.0f);
		scene.set<Rotation>(obj, 0.0, time, 0);
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