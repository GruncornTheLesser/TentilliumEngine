#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include <concepts>
#include <optional>

template<class T, class Handle_t = unsigned int>
class Shared {
public:
	Shared() = default;
	~Shared() {
		destroyRef(m_handle);
	}
	Shared(const Shared<T, Handle_t>& other) {
		createRef(other.m_handle);
		m_handle = other.m_handle;
	}
	Shared<T, Handle_t>& operator=(const Shared<T, Handle_t>& other) {
		if (this->m_handle == other.m_handle)
			return *this;

		destroyRef(m_handle);
		createRef(other.m_handle);
		m_handle = other.m_handle;
		return *this;
	}

	Shared(Shared<T, Handle_t>&& other) {
		m_handle = other.m_handle;
		other.m_handle = 0;
	}
	Shared<T, Handle_t>& operator=(Shared<T, Handle_t>&& other) {
		if (this->m_handle == other.m_handle)
			return *this;

		destroyRef(m_handle);
		m_handle = other.m_handle;
		other.m_handle = 0;

		return *this;
	}

	Handle_t getHandle() const { return m_handle; }

	bool isUnique() const {
		auto it = m_ref_count.find(m_handle);
		return it == m_ref_count.end();
	}

protected:
	Handle_t m_handle = 0;

private:
	static void createRef(Handle_t handle) {
		if (handle == 0) return;

		auto it = m_ref_count.find(handle);
		if (it == m_ref_count.end())
			m_ref_count.emplace_hint(it, handle, 1);
		else
			it->second++;
	}

	static void destroyRef(Handle_t handle) {
		if (handle == 0) return;

		auto it = m_ref_count.find(handle);
		if (it == m_ref_count.end())
			T::destroy(handle);
		else if (--it->second == 0)
			m_ref_count.erase(it);
	}

	inline static std::unordered_map<Handle_t, unsigned int> m_ref_count;
};

template<class T, class Handle_t = std::string>
class ResourceManager {
public:
	void drop(Handle_t handle)
	{
		m_res_cache.erase(handle);
	}

	std::optional<T> fetch(Handle_t handle)
	{
		auto it = m_res_cache.find(handle);
		if (it != m_res_cache.end())
			return T(it->second);

		return std::optional<T>{};
	}

	template<typename ... Ts> requires std::constructible_from<T, Ts...>
	T stash(Handle_t handle, Ts ... args) {
		T value = T(std::move(args)...);
		m_res_cache.emplace(handle, value);
		return value;
	}

	/* gets value associated with 'filepath' or creates new value from args if none is found */
	template<typename ... Ts>
	T fetch_or_default(Handle_t handle, Ts ... args)
	{
		std::optional<T> fetched = fetch(handle);
		if (fetched.has_value()) return fetched.value();
		else				     return stash(handle, args...);
	}

	/* gets value associated with 'handle' or creates new value from filepath if none is found */
	T load(Handle_t handle)
	{
		return fetch_or_default(handle, handle);
	}

	unsigned int size() {
		return m_res_cache.size();
	}

private:
	std::unordered_map<Handle_t, T> m_res_cache;
};