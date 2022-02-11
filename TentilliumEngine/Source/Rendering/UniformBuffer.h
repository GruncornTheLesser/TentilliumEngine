#pragma once

template<typename t>
struct UniformBuffer
{
public:
	unsigned int UBO;
public:
	UniformBuffer(); // default static draw
	
	~UniformBuffer();

	/// <summary>
	/// copies data in 
	/// </summary>
	void setBufferData();

	/// <summary>
	/// returns copy of data stored in uniform buffer of length size()
	/// </summary>
	char* getBufferData();

	void bind();

	unsigned int size();
};
