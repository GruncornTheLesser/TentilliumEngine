#if __INCLUDE_LEVEL__
	#error "Don't include this file"
#endif

#include "UniformBuffer.cpp"
#include "Resources/Material.h"

template struct UniformBuffer<Material>;