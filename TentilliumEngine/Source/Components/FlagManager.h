#include "Flag.h"


struct FlagManager
{
public:
	/*
	Sets all Flags to false.
	This should only ever be called between loops
	remember that this has to apply to multiple instances
	*/
	static void clear()
	{
		Flag::currentVersion++;
		if (!Flag::currentVersion)		// to garentee 0 Flag version is never 0
			Flag::currentVersion++;
	}
};
