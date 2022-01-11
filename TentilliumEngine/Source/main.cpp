#include "Tentil.h"
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <iterator>
using namespace glm;

entt::registry reg;

struct Hierarchy
{
public:
	entity parent;
	int depth;
	
public:
	Hierarchy() {}
	Hierarchy(entity p) : parent(p)
	{
		if (!reg.valid(p))
		{
			std::cerr << "tried to create child to non existent entity." << std::endl;
			throw std::exception();
		}
	}
	Hierarchy(entity parent, int depth) : parent(parent), depth(depth) { }
};

struct Transform
{
public:
	vec3 scale;
	vec3 position;
	quat rotation;
	mat4 matrix;
public:
	Transform() {}
	Transform(vec3 position, vec3 scale = vec3(1), quat rotation = quat(1, 0, 0, 0)) : 
		scale(scale),
		position(position),
		rotation(rotation)
	{ }
	vec3 getWorldPosition()
	{
		return vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
	}
	
};
int main(int argc, char** argv)
{
	auto node0 = reg.create();
	auto node1 = reg.create(); 
	auto node2 = reg.create(); 
	auto node3 = reg.create(); 
	auto node4 = reg.create(); 
	auto node5 = reg.create(); 
	auto node6 = reg.create(); 
	auto node7 = reg.create(); 
	
	// hierarchy
	auto view0 = reg.view<Hierarchy>();

	// transform
	auto obsv0 = observer(reg, collector.update<Transform>());
	auto obsv1 = observer(reg, collector.update<Hierarchy>().where<Transform>());

	auto x = collector.update<Transform>();

	reg.emplace<Hierarchy>(node7);
	reg.emplace<Hierarchy>(node6);
	reg.emplace<Hierarchy>(node5);
	reg.emplace<Hierarchy>(node4);
	reg.emplace<Hierarchy>(node3);
	reg.emplace<Hierarchy>(node2);
	reg.emplace<Hierarchy>(node1);

	reg.emplace<Transform>(node7);
	reg.emplace<Transform>(node6);
	reg.emplace<Transform>(node5);
	reg.emplace<Transform>(node4);
	reg.emplace<Transform>(node3);
	reg.emplace<Transform>(node2);
	reg.emplace<Transform>(node1);
	reg.emplace<Transform>(node0);
	
	reg.replace<Hierarchy>(node7, node5);
	reg.replace<Hierarchy>(node4, node2);
	reg.replace<Hierarchy>(node6, node2);
	reg.replace<Hierarchy>(node5, node2);
	reg.replace<Hierarchy>(node1, node0);
	reg.replace<Hierarchy>(node3, node0);
	reg.replace<Hierarchy>(node2, node0);

	reg.replace<Transform>(node7, vec3(128, -1, 1));
	reg.replace<Transform>(node6, vec3(64, 0, 1));
	reg.replace<Transform>(node5, vec3(32, -1, 1));
	reg.replace<Transform>(node4, vec3(16, 1, 1));
	reg.replace<Transform>(node3, vec3(8, -1, 1));
	reg.replace<Transform>(node2, vec3(4, 1, 1));
	reg.replace<Transform>(node1, vec3(2, -1, 1));
	reg.replace<Transform>(node0, vec3(1, 0, 1));
	
	/*===============Hierarchy System===============*/
	// 2 Hierarchy Operations:
	// adopt:	creates parent/child association by adding a hierarchy component to child
	// unadopt:	removes parent/child association by removing the child's hierarchy component
	// all new components are after the sorted section and parent only to prev
	// [(prev - sorted) 0, 0, 1, 1, 2, 2, 3, ... (new - unsorted*) 4, 2, 3, 1 ...]
	// sub tree can be added in one pass providing that all children are added before the parent
	//	> it would be the other way around but entt iterates backwards

	// remove orphaned(invalid parent) components making them scene roots
	for (auto [entity, hierarchy] : view0.each())
	{
		// check if parent entity has been removed
		if (!reg.valid(hierarchy.parent))
		{
			reg.erase<Hierarchy>(entity);
			reg.patch<Transform>(entity, [](auto& t) {});
		}
	}

	// recalculate hierarchy depth
	for (auto [entity, hierarchy] : view0.each())
	{
		auto temp = hierarchy.depth;

		// if parent has hierarchy component
		if (auto parent = reg.try_get<Hierarchy>(hierarchy.parent))
			hierarchy.depth = 1 + parent->depth;	//  depth = 1 + parent's depth
		else
			hierarchy.depth = 1;

		if (hierarchy.depth != temp)
			if (auto ptr = reg.try_get<Transform>(entity))
				reg.patch<Transform>(entity, [](auto& t) {});


		// if hierarchy changes:
		//		needs to flag transform(if it has transform) for update 
	}

	// resort hierachy components by their depth in the scene graph
	reg.sort<Hierarchy>([](const auto& lhs, const auto& rhs) { return lhs.depth < rhs.depth; });
	reg.sort<Transform, Hierarchy>();

	/*===============Transform System===============*/

	// copy transform order from hierarchy order
	

	// update local transforms
	// obsv0 updates on transform changed
	for (auto entity : obsv0) {
		auto& transform = reg.get<Transform>(entity);
		transform.matrix = 
			glm::mat4(
				vec4(1, 0, 0, 0),
				vec4(0, 1, 0, 0),
				vec4(0, 0, 1, 0),
				vec4(transform.position, 1)
			) *
			(mat4)transform.rotation *
			glm::mat4(
				vec4(transform.scale.x, 0, 0, 0),
				vec4(0, transform.scale.y, 0, 0),
				vec4(0, 0, transform.scale.z, 0),
				vec4(0, 0, 0, 1)
			);

	}
	obsv0.clear();

	// update non root world transform (sorted by scene graph depth)
	// bosv1 updates on hierarchy changed
	for (auto entity : obsv1)
	{
		auto& transform = reg.get<Transform>(entity);
		auto& hierarchy = reg.get<Hierarchy>(entity);
		if (auto parent = reg.try_get<Transform>(hierarchy.parent))
			transform.matrix = parent->matrix * transform.matrix;
	}
	obsv1.clear();

}

// instantiate
//for (int i = 0; i < 10; i++)
//{
//	auto entity = reg.create();
//	position& ref1 = reg.emplace<position>(entity, (float)i, (float)i);
//	velocity& ref2 = reg.emplace<velocity>(entity, (float)i, (float)i);
//	std::cout << "there are " << reg.alive() << " entities alive." << std::endl;
//}
// sort
//reg.sort<position>([](const auto& lhs, const auto& rhs) { return lhs.x > rhs.x; });
//
// iterate
//auto view = reg.view<position, velocity>();
//view.each([](auto& pos, auto& vel) { std::cout << pos.x << std::endl; });
//view.each([](const auto entity, auto& pos, auto& vel) { /* ... */ });
//for (auto [entity, pos, vel] : view.each()) { /* ... */ }

/*
AppWindow* wnd = new AppWindow(640, 480, argv[0], true);
//Scene*	   scn = new Scene;

float vertices[] = {
	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
};

GLuint VBO;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);

glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

GLuint VAO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

// recognise vertex attributes eg position, UV, colour, bone weight and bone index
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);

// load resources from file
auto program = ResourceManager<Shader>::Load("Resources/shaders/Default.shader");
auto img = ResourceManager<Image>::Load("Resources/images/Image.png");

// tells shader to use texture block 1
program->SetUniform1i("tex", 0);
// puts texture into texture block 1
glActiveTexture(GL_TEXTURE0);

img->Bind();

program->Bind();

glBindVertexArray(VAO);

while (!wnd->Closed())
{
	glDrawArrays(GL_TRIANGLES, 0, 6);

	wnd->Refresh();
}

*/