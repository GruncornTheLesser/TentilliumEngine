#pragma once
// redefinition of GL buffer types
#define GL_ARRAY_BUFFER 0x8892
//#define GL_ATOMIC_COUNTER_BUFFER 0x92c0
//#define GL_COPY_READ_BUFFER 0x8f36
//#define GL_COPY_WRITE_BUFFER 0x8f37
//#define GL_DISPATCH_INDIRECT_BUFFER 0x90ee
//#define GL_DRAW_INDIRECT_BUFFER 0x8f3f
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
//#define GL_PIXEL_PACK_BUFFER 0x88eb
//#define GL_PIXEL_UNPACK_BUFFER 0x88ec
//#define GL_QUERY_BUFFER 0x9192
//#define GL_SHADER_STORAGE_BUFFER 0x90d2`
#define GL_TEXTURE_BUFFER 0x8C2A
//#define GL_TRANSFORM_FEEDBACK_BUFFER 0x8c8e
#define GL_UNIFORM_BUFFER 0x8A11



struct GLbuffer
{
protected:
	unsigned int length;
	unsigned int target;
	unsigned int handle;
public:
	GLbuffer(unsigned int type, unsigned int length, void* data = nullptr);

	~GLbuffer();

	template<typename t>
	void assign(t* data) {
		assign(0, sizeof(t), data);
	}

	void assign(unsigned int offset, unsigned int length, void* data);

	template<typename t>
	t* get(const unsigned int offset = 0)
	{
		return (t*)get(offset, sizeof(t));
	}

	char* get(const unsigned int offset, const unsigned int length);

	operator unsigned int() { return handle; }

};

