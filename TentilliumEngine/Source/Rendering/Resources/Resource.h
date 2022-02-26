#pragma once
#include <map>
#include <memory>
#include <string>
// special pointers???
// resources never destructed until static map destroyed
// shared pointer is exactly what i want
// when destructed, invalidating pointers???
template<class t>
struct Resource
{
private:
	inline static std::map<std::string, t> cache;

	static t* Load(std::string filepath);

	template<typename ... args>
	static t* Emplace(std::string filepath, args ... Args)
	{
		auto inserted = cache.emplace(std::piecewise_construct,
			std::forward_as_tuple(filepath),
			std::forward_as_tuple(Args...));
		return &(inserted.first->second);
	}

public:
	/*
	{
	if (auto ptr = Find(filepath))
		return ptr;
	
	// load from file
	
	return Init(filepath, values, to, initiate...);
	}*/
	const static std::shared_ptr<t> Get(std::string filepath)
	{
		std::initializer_list<t> x;

		auto it = cache.find(filepath);
		if (it != cache.end()) 
			return std::shared_ptr<t>(&(it->second));

		t* ptr = Load(filepath);

		return std::shared_ptr<t>(ptr);
	}



	
	/*
	void Save(const char* filepath)
	{
		// do stuff
	}
	
	
	creates copy of loaded resource
	{
		t* ptr = Find(filepath);
		if (!ptr)
			return Load(filepath);
		
		// copy object at ptr

		return Init(filepath, values, to, initiate...);
	}*/
	//const static t* Copy(const char* filepath);
};