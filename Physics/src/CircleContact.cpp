#include "CircleContact.hpp"
#include "Body2D.hpp"
#include "Fixture.hpp"
#include "WorldCallBacks.hpp"
#include "BlockAllocator.hpp"
#include "TimeOfImpact.hpp"

#include <new>


using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;

Contact* CircleContact::Create(Fixture* fixtureA, s32, Fixture* fixtureB, s32, BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(CircleContact));
	return new (mem) CircleContact(fixtureA, fixtureB);
}

void CircleContact::Destroy(Contact* contact, BlockAllocator* allocator)
{
	((CircleContact*)contact)->~CircleContact();
	allocator->Free(contact, sizeof(CircleContact));
}

CircleContact::CircleContact(Fixture* fixtureA, Fixture* fixtureB)
	: Contact(fixtureA, 0, fixtureB, 0)
{
	assert(m_fixtureA->GetType() == Shape::circle);
	assert(m_fixtureB->GetType() == Shape::circle);
}

void CircleContact::Evaluate(Manifold* manifold, const Transform2D& xfA, const Transform2D& xfB)
{
	CollideCircles(manifold,
		(CircleShape*)m_fixtureA->GetShape(), xfA,
		(CircleShape*)m_fixtureB->GetShape(), xfB);
}
