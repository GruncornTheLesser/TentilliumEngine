#pragma once
#include <map>



template<class t>
struct ResourceCache
{
private:
	std::map<const char*, t> cache;
public:
	std::map<const char*, t>* get()
	{
		return &cache;
	}
};



template<class ... t>
struct ResourceManager : ResourceCache<t>...
{
	ResourceManager() { }
	void makeCurrent();
};



template<class t>
struct Resource
{

public:
	inline static std::map<const char*, t>* cache;

public:
	const static t* Load(const char* filepath);
};



template<class ... t>
void ResourceManager<t...>::makeCurrent()
{
	((Resource<t>::cache = (static_cast<ResourceCache<t>*>(this))->get()), ...);
}
