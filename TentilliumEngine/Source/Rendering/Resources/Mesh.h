#pragma once
#include "Resource.h"

typedef void (*callback_setupVertex)();

template<typename ... ts>
void setupVertex()
{
	void setupAttrib(int size, unsigned int stride, unsigned int& no, unsigned int& offset);

	unsigned int stride = (sizeof(ts) + ...);	// sum all sizes of ts
	unsigned int atrribno = 0;					// index of each type
	unsigned int offset = 0;					// pointer to where type starts

	(setupAttrib(sizeof(ts), stride, atrribno, offset), ...);
}

struct Mesh : Resource<Mesh>
{
	friend class Model;
private:
	unsigned int VBO;
	unsigned int IBO;
	callback_setupVertex vertexSetup; // difficult to load
public:
	Mesh(const float* vertexdata, size_t vertexsize, const unsigned int* indexdata, size_t indexsize, callback_setupVertex vertex);
};


