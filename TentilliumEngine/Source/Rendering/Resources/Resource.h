#pragma once
#include <map>
#include <string>
#include <iostream>
// special pointers???
// resources never destructed until static map destroyed
// shared pointer is exactly what i want
// when destructed, invalidating pointers???
template<class t>
class Resource
{
private:
	inline static std::map<const char*, std::weak_ptr<t>> cache;

public:

	const static std::shared_ptr<t> get(const char* filepath) 
	{ 
		auto it = cache.find(filepath);
		if (it != cache.end())
		{
			if(it->second.expired())
			{
				std::cerr << "value for '" << filepath << "' has been unloaded." << std::endl;
				throw std::exception();
			}

			return std::shared_ptr<t>(it->second);
		}
		
		std::cerr << "no value loaded for '" << filepath << "'" << std::endl;
		throw std::exception();
	}

	/* gets value in filepath or if none exists - creates new value from args */
	template<typename ... Ts>
	const static std::shared_ptr<t> get_or_default(const char* filepath, Ts ... args)
	{
		auto it = cache.find(filepath);
		if (it != cache.end() && !(it->second.expired()))
			return std::shared_ptr<t>(it->second);

		return stash(filepath, args...);
	}
	/* creates new value from args in filepath */
	template<typename ... Ts>
	const static std::shared_ptr<t> stash(const char* filepath, Ts ... args)
	{
		auto ptr = std::shared_ptr<t>(new t(args...));
		cache.emplace(filepath, ptr);
		return ptr;
	}

	const static std::shared_ptr<t> load(const char* filepath)
	{
		return get_or_default(filepath, filepath);
	}
};