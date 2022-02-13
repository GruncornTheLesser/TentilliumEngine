#pragma once

template<typename t>
struct UniformBuffer
{
private:
	unsigned int UBO;
public:
	UniformBuffer();

	~UniformBuffer();

public:
	void setBufferData();

	char* getBufferData();

	void bind();
};
