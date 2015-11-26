#include "PolygonContact.hpp"
#include "BlockAllocator.hpp"
#include "TimeOfImpact.hpp"
#include "Body2D.hpp"
#include "Fixture.hpp"
#include "WorldCallBacks.hpp"

#include <new>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


Contact* PolygonContact::Create(Fixture* fixtureA, s32, Fixture* fixtureB, s32, BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(PolygonContact));
	return new (mem) PolygonContact(fixtureA, fixtureB);
}

void PolygonContact::Destroy(Contact* contact, BlockAllocator* allocator)
{
	((PolygonContact*)contact)->~PolygonContact();
	allocator->Free(contact, sizeof(PolygonContact));
}

PolygonContact::PolygonContact(Fixture* fixtureA, Fixture* fixtureB)
	: Contact(fixtureA, 0, fixtureB, 0)
{
	assert(m_fixtureA->GetType() == Shape::polygon);
	assert(m_fixtureB->GetType() == Shape::polygon);
}

void PolygonContact::Evaluate(Manifold* manifold, const Transform2D& xfA, const Transform2D& xfB)
{
	CollidePolygons(	manifold,
		(PolygonShape*)m_fixtureA->GetShape(), xfA,
		(PolygonShape*)m_fixtureB->GetShape(), xfB);
}
