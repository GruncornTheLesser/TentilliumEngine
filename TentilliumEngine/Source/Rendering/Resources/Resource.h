#pragma once
#include <map>
#include <string>
#include <iostream>
// special pointers???
// resources never destructed until static map destroyed
// shared pointer is exactly what i want
// when destructed, invalidating pointers???
template<class t>
struct Resource
{
private:
	inline static std::map<const char*, std::weak_ptr<t>> cache;

public:
	template<typename ... Ts>
	const static std::shared_ptr<t> get_emplace(const char* filepath, Ts ... args)
	{
		auto it = cache.find(filepath);
		if (it != cache.end() && !(it->second.expired()))
			return std::shared_ptr<t>(it->second);

		return stash(filepath, new t(args...));
	}

	const static std::shared_ptr<t> get(const char* filepath)
	{
		auto it = cache.find(filepath);
		if (it != cache.end() && !(it->second.expired()))
			return std::shared_ptr<t>(it->second);
		
		return stash(filepath, new t(filepath));
	}

	const static std::shared_ptr<t> stash(const char* filepath, t* resource)
	{
		auto ptr = std::shared_ptr<t>(resource);
		cache.emplace(filepath, ptr);
		return ptr;
	}
};