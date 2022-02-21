#pragma once
#include <map>

template<class t>
struct Resource
{
private:
	inline static std::map<const char*, t> cache;

protected:
	const static t* Find(const char* filepath)
	{
		auto it = cache.find(filepath);
		if (it == cache.end()) return 0;
		return &(it->second);
	}

	template<typename ... types>
	const static t* Init(const char* filepath, types ... initList) {
		cache.emplace(std::piecewise_construct, std::forward_as_tuple(filepath), std::forward_as_tuple(initList...));
		return &cache.at(filepath);
	}

public:
	/*
	{
	if (auto ptr = Find(filepath))
		return ptr;
	
	// load from file
	
	return Init(filepath, values, to, initiate...);
	}*/
	const static t* Get(const char* filepath)
	{
		if (auto ptr = Find(filepath))
			return ptr;
		else
			return Load(filepath);
	}

	const static t* Load(const char* filepath);

	/*
	{
		t* ptr = Find(filepath);
		if (!ptr)
			return Load(filepath);
		
		// copy object at ptr

		return Init(filepath, values, to, initiate...);
	}*/
	//const static t* Copy(const char* filepath);
};