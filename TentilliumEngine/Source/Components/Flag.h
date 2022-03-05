#pragma once

struct Flag
{
	friend class FlagManager;
private:
	static inline unsigned int currentVersion = 1;	// starts at 1
	unsigned int version;							

public:
	void Lower() { version = 0; }					// 0 is garenteed to be invalid
	void Raise() { version = currentVersion; }
	operator bool() { return version == currentVersion; }
};
