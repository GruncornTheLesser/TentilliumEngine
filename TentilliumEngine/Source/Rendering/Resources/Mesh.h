#pragma once
#include "GLbuffer.h"

// MESH CLASS

// INTERLEAVED - attributes in the same buffer
// SEPERATED - attributes in seperate buffers

// INTERLEAVED vs SEPERATED
// seperate is better if single values of vertices change
// too many buffers is bad, long buffers are also bad

// DIRECT - data is stored in an array and called upon as needed
// INDEXED - unique data is stored in one buffer and a different buffer stores an index pointing to the first buffer

// INDEXED vs DIRECT
// indexed is better, uses less memory 
// can sometimes use fast cache


// Current structure:
//			Model
//			  |
//	   MeshInstance(VAO)[N]
//		  |		 |
//Material(UBO)	 Mesh(VBO)
//    | 
// Textures
// 
// Issues
//  > annoying to render entire mesh as all the vertex groups are seperate

// VIEW(viewRender, GET(Mesh, Render), EXC())
// Proposed structure:
//    Mesh(VBOs)	Renderer(VAO)	Bones(UBO)
//       |              |				|
// VertGroup[N]   Material(UBO)[N]	 entities
//	                    |
//					Textures
//
// Advantages:
//  > more functionality -> can draw wire mesh with no render material
//	> can adjust mesh per program to include or exclude vertex data
//	> more data oriented allowing easier repurposing of data

enum class VertAttrib { Index, Position, TexCoord, Normal, BoneID, BoneWeight };

template<VertAttrib>
class VBO : public GLbuffer {
public:
	VBO(void* data, size_t size) : GLbuffer(data, size) { }
	// TODO:
	//template<typename T>
	//VBO(std::vector<T>& data) : GLbuffer(&data[0], sizeof(data)) { }
};

