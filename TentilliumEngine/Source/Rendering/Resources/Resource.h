#pragma once
#include <map>
#include <string>
#include <iostream>

template<class t>
class Resource
{
private:
	inline static std::map<std::string, std::weak_ptr<t>> cache;

public:
	const static std::shared_ptr<t> get(std::string filepath) 
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
	const static std::shared_ptr<t> get_or_default(std::string filepath, Ts ... args)
	{
		auto it = cache.find(filepath);
		if (it != cache.end() && !(it->second.expired()))
			return std::shared_ptr<t>(it->second);

		return stash(filepath, args...);
	}
	/* creates new value from args in filepath */
	template<typename ... Ts>
	const static std::shared_ptr<t> stash(std::string filepath, Ts ... args)
	{
		std::shared_ptr<t> ptr(new t(args...));
		cache.emplace(filepath, ptr);
		return ptr;
	}

	const static std::shared_ptr<t> load(std::string filepath)
	{
		return get_or_default(filepath, filepath);
	}
};