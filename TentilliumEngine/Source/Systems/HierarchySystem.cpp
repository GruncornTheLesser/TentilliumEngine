#include "HierarchySystem.h"

void HierarchySystem::update()
{
	for (auto it = viewHierarchy.rbegin(); it != viewHierarchy.rend();)
	{
		Hierarchy& hierarchy = get<Hierarchy>(*it);

		if (!valid(hierarchy.m_parent) || hierarchy.m_parent == *it)
		{
			erase<Hierarchy>(*it);
			continue;
		}

		Hierarchy* hierachyParent = try_get<Hierarchy>(hierarchy.m_parent);

		if (hierachyParent && &hierarchy > hierachyParent)
			viewHierarchy.storage<Hierarchy>().swap_elements(*it, hierarchy.m_parent);

		it++;
	}
}
