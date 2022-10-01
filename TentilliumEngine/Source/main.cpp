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
*		> 
*	> MegaTextures -> copy all textures onto one giant 8k texture
*		> store all materials into giant shared buffer
*		> store only uv and material index data in geometryBuffer
*		
*	> Decals ???
*	> probes ??? -> basically used for reflection maps, the scene is rendered to a cubemap
* 
*	> PointLight position, scale and (rotation + projection) association as well as directional + spotlights
*	> Normal + Tangent VBO generation
*	> decouple Material into maps and 
*	> seperate rendering functionality further for mesh/camera/rendering type
* 
* 
*	> Tidy AppWindow functionality into something less ugly
*		> rethink input system
*	
*	> Tidy Texture and rethink the scope of the class. 
*		Texture is used in 2 places currently: Materials and Framebuffers
*		Material desperately needs some way to pass multiple textures This can be done through
*			> Megatextures/texture atlases
*			> Texture Arrays
*				> means material initation cant use the current texture file loading
*			> bindless textures
* 
* 
*   > bones, skinning and animation
*/

/* RESEARCH:
*	> Morton encoding for position in a quad tree -> chunking???
*	> ANGLE a library that compiles opengl calls into directx
*	> bindless textures
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
	entt::entity light1, light2;

	float time;

	std::vector<entt::entity> lights;

	TestApp(const char* title) : AppWindow(800, 600, title)
	{
		scene.size = AppWindow::size;

		// create camera (entity 0)
		{ 
			entt::entity cam = scene.create();
			scene.set<Projection>(cam, glm::radians(60.0f), 800.0f / 600.0f, 0.02f, 100.0f);
			scene.set<Position>(cam, 0, 1, 0);
			scene.set<Rotation>(cam);
			scene.camera = cam;
		}

		// create obj (entity 1)
		{
			obj = scene.load("Resources/meshes/Person.fbx");
			scene.set<Position>(obj, 0, 0, -1);
			scene.set<Rotation>(obj);
		}

		// create box1 (entity 2)
		{
			box1 = scene.load("Resources/meshes/ColourCube.fbx");

			scene.set<Parent>(box1, obj);
			scene.set<Position>(box1, -0.5, 0, 0);
			scene.set<Scale>(box1, 0.5f);

			scene.set<Material>(box1, glm::vec4(1, 1, 1, 1), 0.0f, 0.0f, Texture("Resources/textures/bricks_normal.png"));
		}

		// create box2 (entity 3)
		/*
		{
			box2 = scene.create();
			scene.set<Parent>(box2, box1);
			
			scene.set<Position>(box2, 0.5, 0, 0);
			scene.set<Scale>(box2, 0.5f);
			
			scene.set<VBO<V_Index>>(box2, scene.get<VBO<V_Index>>(box1));
			scene.set<VBO<V_Position>>(box2, scene.get<VBO<V_Position>>(box1));
			scene.set<VBO<V_Normal>>(box2, scene.get<VBO<V_Normal>>(box1));
			scene.set<VBO<V_Tangent>>(box2, scene.get<VBO<V_Tangent>>(box1));
			scene.set<VBO<V_TexCoord>>(box2, scene.get<VBO<V_TexCoord>>(box1));
			
			scene.set<Material>(box2, Texture(2, 2, Texture::Format::RGB, true, RGBtest, Texture::Format::RGB, Texture::Type::UNSIGNED_BYTE));
		}
		*/
		// floor (entity 4)
		{
			entt::entity floor = scene.create();
			
			scene.set<Position>(floor, 0, -0.1f, 0);
			scene.set<Scale>(floor, 100, 0.1f, 100);
			
			scene.set<VBO<V_Index>>(floor, scene.get<VBO<V_Index>>(box1));
			scene.set<VBO<V_Position>>(floor, scene.get<VBO<V_Position>>(box1));
			scene.set<VBO<V_Normal>>(floor, scene.get<VBO<V_Normal>>(box1));
			scene.set<VBO<V_Tangent>>(floor, scene.get<VBO<V_Tangent>>(box1));
			scene.set<VBO<V_TexCoord>>(floor, scene.get<VBO<V_TexCoord>>(box1));
			
			scene.set<Material>(floor, Texture("Resources/textures/grid.png"));
		}

		// light 1 (entity 5)
		{
			light1 = scene.create();
			scene.set<PointLight>(light1, glm::vec3(0, 1, 0), glm::vec3(2, 2, 2), 10.0f);
		}
		/*
		// light 2 (entity 6)
		{
			light2 = scene.create();
			scene.set<PointLight>(light2, glm::vec3(-0.5f, 0, 0), glm::vec3(0, 2, 2), 10.0f);
			scene.set<Position>(light2, -0.5f, 0, 0);
			scene.set<Scale>(light2, 10.0f);
		}
		*/
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

			scene.set<Rotation>(scene.camera, camera_rotation);
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
			scene.set<Position>(scene.camera, camera_rotation * move_direction * delta + camera_position); // rotate direction by camera rotation
		

		scene.set<Position>(box1, cos(time), sin(time), 0.0f);
		//scene.set<Position>(box2, -sin(time), -cos(time), 0.0f);
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