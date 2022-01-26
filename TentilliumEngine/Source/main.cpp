#include "Tentil.h"
#include <iostream>
#include <queue>
#include "Components/Scene.h"

int main(int argc, char** argv)
{
	struct A { int test; };
	struct B { int test; };
	struct C { };

	auto g = reg.group<A, B>();
	auto o = observer{ reg, collector.update<A>().update<B>().where<A, B>() };

	auto t0 = reg.create();
	reg.emplace<A>(t0);
	reg.emplace<B>(t0);
	auto t1 = reg.create();
	reg.emplace<A>(t1);
	reg.emplace<B>(t1);
	auto t2 = reg.create();
	reg.emplace<A>(t2);
	reg.emplace<B>(t2);
	auto t3 = reg.create();
	reg.emplace<A>(t3);
	reg.emplace<B>(t3);

	g.sort<A>([](const A& lhs, const A& rhs) { return rand() & 1 == 1; });

	std::cout << "random entity order" << std::endl;
	for (auto entity : g)
		std::cout << (int)entity << std::endl;

	o.clear();

	reg.emplace<C>(t0);
	reg.emplace<C>(t1);
	reg.emplace<C>(t2);
	reg.emplace<C>(t3);

	std::cout << "observer order" << std::endl;
	for (auto entity : o)
		std::cout << (int)entity << std::endl;



	auto e0 = reg.create();
	auto e1 = reg.create();
	auto e2 = reg.create();
	auto e3 = reg.create();
	auto e4 = reg.create();
	auto e5 = reg.create();
	auto e6 = reg.create();
	auto e7 = reg.create();

	// components must be initiated such that the components parent is before the child
	// however because the entt storage iterates backwards it means that the component 
	// order is reversed. bottom line:
	// components parents must be sorted after their children or the system will break
	Scene<0> scn;
	scn.Add(e7, Hierarchy(e4), Transform(vec3(8, 1, 2)));
	scn.Add(e6, Hierarchy(e4), Transform(vec3(7, 1, 2)));
	scn.Add(e5, Hierarchy(e4), Transform(vec3(6, 1, 2)));
	scn.Add(e4, Hierarchy(e1), Transform(vec3(5, 1, 2)));
	scn.Add(e3, Hierarchy(e0), Transform(vec3(4, 1, 1)));
	scn.Add(e2, Hierarchy(e0), Transform(vec3(3, 1, 1)));
	scn.Add(e1, Hierarchy(e0), Transform(vec3(2, 1, 1)));
	scn.Add(e0, Transform(vec3(1, 1, 0)));	
	
	scn.HierarchySystem();
	scn.TransformSystem();
	
	AppWindow* wnd = new AppWindow(640, 480, argv[0], true);

	// vertex data
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
	};
	unsigned int indices[] = { 0, 1, 2, 2, 1, 3 }; // mesh indices

	GLuint VAO; // vertex array object - container for buffers and vertex data
	GLuint VBO; // vertex buffer object - all unique vertices
	GLuint IBO; // index buffer object - array of pointers to VBO indices to spell out primitives eg Triangles
	// generate buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	// bind buffers
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	// set buffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// recognise vertex attributes eg position, UV, colour, bone weight and bone index
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// load resources from file
	auto program = ResourceManager<Shader>::Load("Resources/shaders/Default.shader");
	auto img = ResourceManager<Image>::Load("Resources/images/Image.png");

	
	program->SetUniform1i("tex", 0);	// tells shader to use texture block 0
	glActiveTexture(GL_TEXTURE0);		// selects which texture unit subsequent texture state calls will affect. 
	img->Bind();						// subsequent texture state call. ie adds img texture to GL_TEXTURE0

	program->Bind();					// use shader program

	glBindVertexArray(VAO);				// use mesh

	while (!wnd->Closed())
	{
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // draw mesh

		wnd->Refresh();
	}

	
	// ...
	// reg.emplace<Hierarchy>(entity, args...); // 1. add hierarchy component to add new relationship
	// reg.erase<Hierarchy>(entity);			// 2. remove hierarchy component to remove old relationship
	// reg.patch<Hierarchy>(entity, reparent);	// 3. [NOT ALLOWED] change value of hierarchy component
	//											// 4. no change to hierarchy
	// reg.emplace<Transform>(entity, args...);	// 5. add transform component
	// reg.erase<Transform>(entity);			// 6. remove transform component
	// reg.patch<Transform>(entity, transform); // 7. change value of transform component
	//											// 8. no change to transform
	// ...
	// 
	// [no change(4) + changed(3) + orphaned(2)..., new(1)...]
	//
}
/*
	auto view0 = reg.view<Hierarchy>();


	// transform
	auto obsv0 = observer(reg, collector.update<Transform>()); // observe user changes to transform
	auto obsv1 = observer(reg, collector.update<Hierarchy>().where<Transform>()); // observe changes made by hierarchy

	auto node0 = reg.create();
	auto node1 = reg.create();
	auto node2 = reg.create();
	auto node3 = reg.create();
	auto node4 = reg.create();
	auto node5 = reg.create();
	auto node6 = reg.create();
	auto node7 = reg.create();

	reg.emplace<Hierarchy>(node7);
	reg.emplace<Transform>(node7);

	reg.emplace<Hierarchy>(node6);
	reg.emplace<Transform>(node6);

	reg.emplace<Hierarchy>(node5);
	reg.emplace<Transform>(node5);

	reg.emplace<Hierarchy>(node4);
	reg.emplace<Transform>(node4);

	reg.emplace<Hierarchy>(node3);
	reg.emplace<Transform>(node3);

	reg.emplace<Hierarchy>(node2);
	reg.emplace<Transform>(node2);

	reg.emplace<Hierarchy>(node1);
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

	//===============Hierarchy System===============
	// 2 Hierarchy Operations:
	// adopt:	creates parent/child association by adding a hierarchy component to child
	// unadopt:	removes parent/child association by removing the child's hierarchy component
	// all new components are after the sorted section and parent only to prev
	// [(prev - sorted) 0, 0, 1, 1, 2, 2, 3, ... (new - unsorted*) 4, 2, 3, 1 ...]
	// sub tree can be added in one pass providing that all children are added before the parent
	//	> it would be the other way around but entt iterates backwards

	// remove orphaned components(invalid parent) making them scene roots
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

	//===============Transform System===============

	// copy order from hierachy to transform
	reg.sort<Transform, Hierarchy>();


	// update local transforms and root world transform
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
	// obsv1 updates on hierarchy changed
	// this fucks up and wont update transforms with no hierarchy change
	for (auto entity : obsv1)
	{
		auto& transform = reg.get<Transform>(entity);
		auto& hierarchy = reg.get<Hierarchy>(entity);
		if (auto parent = reg.try_get<Transform>(hierarchy.parent))
			transform.matrix = parent->matrix * transform.matrix;
	}
	obsv1.clear();
	*/
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
struct A { int t; };
struct B { int e; };
struct C { int s; };
struct D { int t; };

// partial ownership of B full ownership of A
auto g1 = reg.group<A>(get<B>);

// iterate over component that have both A and B
const auto begin = g1.storage<A>().data();
const auto end = g1.storage<A>().data() + g1.size();
std::for_each(begin, end, [raw = g1.storage<A>().raw()](auto entity) mutable {
	auto& component = *(raw++);
	});

// iterate over components that have only A not B
const auto begin = g1.storage<A>().data() + g1.size();
const auto end = g1.storage<A>().data() + g1.storage<A>().size();
std::for_each(begin, end, [raw = g1.storage<A>().raw() + g1.size()](auto entity) mutable {
	auto& compA = *(raw++);
	});
auto g2 = reg.group<C, D>();
g2.each([](auto& compC, auto& compD) {});

// get component C of entities with only C
const auto begin = g2.storage<C>().raw() + g2.size();
const auto end = g2.storage<C>().raw() + g2.storage<C>().size();
std::for_each(begin, end, [](auto& compC) {});

// get component D of entities with only D
const auto begin = g2.storage<D>().raw() + g2.size();
const auto end = g2.storage<D>().raw() + g2.storage<D>().size();
std::for_each(begin, end, [](auto& compD) {});
*/