#pragma once

struct Flag
{
	friend class FlagManager;
private:
	static inline unsigned int currentVersion = 0;
	unsigned int version;

public:
	void Raise() { version = currentVersion; }
	operator bool() { return version == currentVersion; }
};
