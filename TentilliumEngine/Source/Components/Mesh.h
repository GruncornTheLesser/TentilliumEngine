#pragma once
#include "../Rendering/Resources/GLbuffer.h"
#include <vector>
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

namespace Mesh {

	enum VertAttrib { V_Index = -1, V_Position, V_Normal, V_TexCoord, V_BoneID, V_BoneWeight, V_Custom, V_None, };

	template<VertAttrib>
	class VBO final : public GLbuffer {
		friend class VAO;
	public:
		VBO(const void* data, size_t size) : GLbuffer(data, size) { }

		template<typename T>
		VBO(const std::vector<T>& data) : GLbuffer(&data[0], sizeof(T) * data.size()) { }
	};

	class VAO {
	public:
		__declspec(property(get = get_handle)) unsigned int handle;

		template<VertAttrib ... attribs>
		VAO(VBO<attribs>* ... buffers)
		{
			genVAO();
			(attach(buffers), ...);
		}
		~VAO();

		VAO(const VAO&) = delete;
		VAO& operator=(const VAO&) = delete;

		VAO(VAO&&);
		VAO& operator=(VAO&&);

		void draw(int primitive = 0x0004, int size = 0) const;

		template<VertAttrib attrib>
		void attach(VBO<attrib>* buffer);

		template<VertAttrib attrib>
		void detach();

		void attach(int attrib_no, GLbuffer* buffer_handle, int size, int type = 0x1406, bool normalized = false, int stride = 0);

		void detach(int attrib_no);

		unsigned int get_handle() const { return m_handle; }
	private:
		void genVAO();

		void findSize();

		unsigned int m_handle;
		int m_size;
		VertAttrib m_size_attribute = V_None;
};
}