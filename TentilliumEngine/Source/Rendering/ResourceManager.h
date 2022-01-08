#pragma once
#include <map>

template<class t>
struct ResourceManager
{
private:
	inline static std::map<const char*, t> cache;
public:
	const static t* Load(const char* filepath);
};