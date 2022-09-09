#pragma once
#include <map>
#include <string>
#include <iostream>
#include <concepts>

// used for:
// textures
// models
// shader

template<class T>
class Resource 
{
private:
	inline static std::map<std::string, unsigned int> m_file_cache;
	inline static std::map<unsigned int, unsigned int> m_ref_count;

protected:
	/* use in constructors, reports new instance of handle */
	static void create(unsigned int handle) {
		if (handle == 0)
			return;

		auto it = m_ref_count.find(handle);
		if (it != m_ref_count.end())
			it->second++;
		else
			m_ref_count.emplace(std::pair(handle, 1));
	}
	
	/* use in destructor, returns true if all copies of handle destroyed */
	static bool destroy(unsigned int handle) {
		if (handle == 0)
			return false;

		auto it = m_ref_count.find(handle);
		
		if (it == m_ref_count.end())
			return false;

		it->second--;

		if (it->second == 0)
		{
			m_ref_count.erase(it->first);
			return true;
		}

		return false;
	}
	
	/* returns true if ref count equals 0 */
	static bool expired(unsigned int handle)
	{
		if (handle == 0)
			return true;

		auto it = m_ref_count.find(handle);
		return it == m_ref_count.end();
	}

public:
	/*gets value associated with 'filepath', throws exception if none is found*/
	const static T get(std::string filepath)
	{
		auto it = m_file_cache.find(filepath);
		if (it != m_file_cache.end() && !expired(it->second))
			return T(it->second);

		std::cerr << "[Resource Error] - " << "failed to get " << typeid(T).name() << " from '" << filepath << "'" << std::endl;
		throw std::exception();
	}

	/* gets value associated with 'filepath' or creates new value from args if none is found */
	template<typename ... Ts> requires std::constructible_from<T, Ts...>
	const static T get_or_default(std::string filepath, Ts ... args)
	{
		auto it = m_file_cache.find(filepath);
		if (it != m_file_cache.end() && !expired(it->second))
			return T(it->second);

		T value = T(std::move(args)...);
		m_file_cache.emplace(filepath, value.m_handle);
		return value;
	}
	
	/* gets value associated with 'filepath' or creates new value from filepath if none is found */
	const static T load(std::string filepath)
	{
		return get_or_default(filepath, filepath);
	}
};

