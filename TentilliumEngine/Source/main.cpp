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
*/

/* TODO:
*	> Tidy AppWindow functionality into something less ugly
*	> Tidy Shader functionality into something less ugly
*
*   > forward+ render pipeline
*		-> depth pre pass		*DONE*
*		-> light culling		*DONE*
*		-> light accumulation	*DONE*
*		-> FrameBuffer
*		-> c++ function
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

	std::vector<entt::entity> lights;

	TestApp(const char* title) : AppWindow(200, 200, title), scene(200, 200)
	{
		root = scene.create();

		// create camera
		entt::entity cam = scene.create();
		scene.emplace<Projection>(cam, glm::radians(60.0f), 800.0f / 600.0f, 0.02f, 10.0f);
		scene.emplace<Transform>(cam, glm::vec3(0, 0, 0));
		scene.camera = cam;

		// create obj
		obj = scene.load("Resources/meshes/Person.fbx");
		scene.emplace<Hierarchy>(obj, root);
		scene.emplace<Transform>(obj, glm::vec3(0, -10, -2), glm::vec3(10));

		// create box1
		box1 = scene.create();
		scene.emplace<Hierarchy>(box1, root);
		scene.emplace<Transform>(box1, glm::vec3(-0.5, 0, 0), glm::vec3(0.5f));
		scene.emplace<VBO<Index>>(box1, indices);
		scene.emplace<VBO<Position>>(box1, positions);
		scene.emplace<VBO<Normal>>(box1, normals);
		scene.emplace<VBO<TexCoord>>(box1, texCoords);
		scene.emplace<Material>(box1, Texture("Resources/textures/pigeon.jpg"));

		// create box2
		box2 = scene.create();
		scene.emplace<Hierarchy>(box2, box1);
		scene.emplace<Transform>(box2, glm::vec3(0.5, 0, 0), glm::vec3(0.5f));
		scene.emplace<VBO<Index>>(box2, scene.get<VBO<Index>>(box1));
		scene.emplace<VBO<Position>>(box2, scene.get<VBO<Position>>(box1));
		scene.emplace<VBO<Normal>>(box2, scene.get<VBO<Normal>>(box1));
		scene.emplace<VBO<TexCoord>>(box2, scene.get<VBO<TexCoord>>(box1));
		scene.emplace<Material>(box2, Texture(RGBtest, 2, 2, Texture::Format::RGB));
		int x = Resource<Texture>::count(2);
		/*
		// light 1
		entt::entity light1 = scene.create();
		scene.emplace<PointLight>(light1, glm::vec3(0, 0, -0.5f), glm::vec3(1, 0, 1), 5.0f);
		scene.emplace<Transform>(light1, glm::vec3(0, 0, -0.5f), glm::vec3(0.1f));
		scene.emplace<VBO<Index>>(light1, scene.get<VBO<Index>>(box1));
		scene.emplace<VBO<Position>>(light1, scene.get<VBO<Position>>(box1));
		scene.emplace<VBO<Normal>>(light1, scene.get<VBO<Normal>>(box1));
		scene.emplace<VBO<TexCoord>>(light1, scene.get<VBO<TexCoord>>(box1));
		scene.emplace<Material>(light1, glm::vec4(1, 0, 1, 1));
		*/
		// light 2
		entt::entity light2 = scene.create();
		scene.emplace<PointLight>(light2, glm::vec3(0.5f, 0, 0), glm::vec3(1, 0, 0), 5.0f);
		scene.emplace<Transform>(light2, glm::vec3(0.5f, 0, 0), glm::vec3(0.1f));
		scene.emplace<VBO<Index>>(light2, scene.get<VBO<Index>>(box1));
		scene.emplace<VBO<Position>>(light2, scene.get<VBO<Position>>(box1));
		scene.emplace<VBO<Normal>>(light2, scene.get<VBO<Normal>>(box1));
		scene.emplace<VBO<TexCoord>>(light2, scene.get<VBO<TexCoord>>(box1));
		scene.emplace<Material>(light2, glm::vec4(1, 0, 0, 1));

		// light 3
		entt::entity light3 = scene.create();
		scene.emplace<PointLight>(light3, glm::vec3(-0.5f, 0, 0), glm::vec3(0, 0, 1), 500.0f);
		scene.emplace<Transform>(light3, glm::vec3(-0.5f, 0, 0), glm::vec3(0.1f));
		scene.emplace<VBO<Index>>(light3, scene.get<VBO<Index>>(box1));
		scene.emplace<VBO<Position>>(light3, scene.get<VBO<Position>>(box1));
		scene.emplace<VBO<Normal>>(light3, scene.get<VBO<Normal>>(box1));
		scene.emplace<VBO<TexCoord>>(light3, scene.get<VBO<TexCoord>>(box1));
		scene.emplace<Material>(light3, glm::vec4(0, 0, 1, 1));

		// floor
		entt::entity floor = scene.create();
		scene.emplace<Transform>(floor, glm::vec3(0, -0.1f, 0), glm::vec3(100, 0.1f, 100));
		scene.emplace<VBO<Index>>(floor, scene.get<VBO<Index>>(box1));
		scene.emplace<VBO<Position>>(floor, scene.get<VBO<Position>>(box1));
		scene.emplace<VBO<Normal>>(floor, scene.get<VBO<Normal>>(box1));
		scene.emplace<VBO<TexCoord>>(floor, scene.get<VBO<TexCoord>>(box1));
		scene.emplace<Material>(floor, glm::vec4(1.0f, 1.0f, 1.0f, 1));
	}

	void onProcess(float delta) {

		if (scene.valid(box1)) scene.get<Transform>(box1).position = glm::vec3(cos(time), sin(time), -1);
		if (scene.valid(box2)) scene.get<Transform>(box2).position = glm::vec3(-sin(time), -cos(time), -1);

		time += delta;

		auto& cam_trans = scene.get<Transform>(scene.camera);
		
		if (isPressed(Button::LEFT))
			cam_dir += (glm::vec2)m_mouse.getDelta() / glm::vec2(getSize());
		cam_trans.rotation = glm::quat(glm::vec3(cam_dir.y, cam_dir.x, 0));
		
		glm::vec3 move_direction{ 0, 0, 0 };
		if (isPressed(Key::W)) move_direction.z -= 1;
		if (isPressed(Key::S)) move_direction.z += 1;
		if (isPressed(Key::A)) move_direction.x -= 1;
		if (isPressed(Key::D)) move_direction.x += 1;
		if (isPressed(Key::F)) move_direction.y -= 1;
		if (isPressed(Key::R)) move_direction.y += 1;

		
		if (isPressed(Key::LEFT_SHIFT)) {
			move_direction *= 10;
		}
		if (isPressed(Key::LEFT_CONTROL)) {
			move_direction *= 0.1;
		}
		cam_trans.position += cam_trans.rotation * move_direction * delta; // rotate direction by camera rotation

	}

	void onDraw() {
		scene.process();
		refresh();
	}

	void onResize(glm::ivec2 size) {
		scene.resize(size);
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