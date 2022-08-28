#include "HierarchySystem.h"

// hierarchy order goes from:
// child,		parent
// [end, ..., begin]
//      <---- forward iteration
// [rbegin, ..., rend]
//		----> reverse iteration
// when a child is reached before its parent; their order is swapped to ensure the parent is always reached first
// this may invalidate children of the swapped child but that will be handled when those children are reached.
// no children can be invalidated from moving the parent earlier in the array.
// 
// when an entity is erased from the hierarchy, entt fills that with a swap and pop policy:
// [begin, ..., erased, ..., end1, end2]
// [begin, ..., end2, ...,end1]
// 
// this ruins the order on components already sorted by this system.
// [unsorted, ..., erased, ..., sorted1, sorted2]
// [unsorted, ..., sorted2, ..., sorted1]
// 
// to prevent this, I sort in reverse:
// [sorted, ... erased, ..., end1, end2]
// [sorted, ..., end2, ..., end1]
// 
// after a parent and a child have been swapped:
// [begin, ..., child, ..., parent, ..., end]
//							   ^
// there is still a chance that this parents parent is 

void HierarchySystem::update()
{
	for (auto it = viewHierarchy.rbegin(); it != viewHierarchy.rend();)
	{
		Hierarchy& hierarchy = get<Hierarchy>(*it);

		if (!valid(hierarchy.parent) || hierarchy.parent == *it)
		{
			erase<Hierarchy>(*it);
			continue;
		}

		Hierarchy* hierachyParent = try_get<Hierarchy>(hierarchy.parent);

		if (hierachyParent && &hierarchy > hierachyParent) {
			viewHierarchy.storage<Hierarchy>().swap_elements(*it, hierarchy.parent);
			continue;
		}
		it++;
	}
}
