#include "WorldCallBacks.hpp"
#include "Fixture.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;



bool ContactFilter::ShouldCollide(Fixture* fixtureA, Fixture* fixtureB)
{
	const Filter& filterA = fixtureA->GetFilterData();
	const Filter& filterB = fixtureB->GetFilterData();

	if (filterA.groupIndex == filterB.groupIndex && filterA.groupIndex != 0)
	{
		return filterA.groupIndex > 0;
	}

	bool collide = (filterA.maskBits & filterB.categoryBits) != 0 && (filterA.categoryBits & filterB.maskBits) != 0;
	return collide;
}
