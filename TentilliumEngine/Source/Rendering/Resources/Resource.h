#pragma once
#include <map>
#include <string>
#include <iostream>
#include <concepts>

// used for:
// textures
// models
// 
//

template<class T>
class Resource
{
private:
	inline static std::map<std::string, std::weak_ptr<T>> cache;

public:
	const static std::shared_ptr<T> get(std::string filepath) 
	{ 
		auto it = cache.find(filepath);
		if (it != cache.end())
		{
			if(it->second.expired())
				return nullptr;

			return std::shared_ptr<T>(it->second);
		}
		
		return nullptr;
	}

	/* gets value in filepath or if none exists - creates new value from args */
	template<typename ... Ts>
	const static std::shared_ptr<T> get_or_default(std::string filepath, Ts ... args)
		requires std::constructible_from<T, Ts...>
	{
		auto it = cache.find(filepath);
		if (it != cache.end() && !(it->second.expired()))
			return std::shared_ptr<T>(it->second);

		return stash(filepath, args...);
	}
	/* creates new value from args in filepath */
	template<typename ... Ts>
	const static std::shared_ptr<T> stash(std::string filepath, Ts ... args)
		requires std::constructible_from<T, Ts...>
	{
		std::shared_ptr<T> ptr(new T(std::move(args)...));
		cache.emplace(filepath, ptr);
		return ptr;
	}
	
	const static std::shared_ptr<T> load(std::string filepath)
		requires std::constructible_from<T, std::string>
	{
		return get_or_default(filepath, filepath);
	}
};

