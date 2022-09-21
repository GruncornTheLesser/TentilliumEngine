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
// this ruins the order on components already sorted by this system.
// [unsorted, ..., erased, ..., sorted1, sorted2]
// [unsorted, ..., sorted2, ..., sorted1]
// 
// to prevent this, I sort in reverse:
// [sorted, ... erased, ..., end1, end2]
// [sorted, ..., end2, ..., end1]
// after a parent and a child have been swapped:
// [begin, ..., child, ..., parent, ..., end]
// there is still a chance that this parent's parent is in the wrong order. There are a couple of options:
//		> ignore the swapped parent, skip and let it update in the next iteration
//		> update the parent until it doesnt need swapping
//		> update the parent a fixed number of times until either it doesnt need swapping or the fixed number is up

void HierarchySystem::update()
{
	for (auto it = viewHierarchy.rbegin(); it != viewHierarchy.rend();) {	// iterate over Hierarchies in reverse
		Parent& hierarchy = get<Parent>(*it);							// get Hierarchy

		if (!valid(hierarchy) || hierarchy == *it) {				// if parent invalid or parent of itself
			erase<Parent>(*it);										// this does not capture all edge cases 
			continue;												// eg if a graph contains a loop the system will break			
		}

		Parent* hierachyParent = try_get<Parent>(hierarchy);				// if hierarchy child(current) is being processed before
		if (hierachyParent && &hierarchy > hierachyParent) {				// it's parent; swap their positions
			viewHierarchy.storage<Parent>().swap_elements(*it, hierarchy);
			continue;														// by continuing it skips the iteration
		}

		it++;
	}
}
