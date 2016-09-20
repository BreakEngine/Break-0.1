#include "PolygonCircleContact.hpp"
#include "BlockAllocator.hpp"
#include "Fixture.hpp"

#include <new>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;


Contact* PolygonAndCircleContact::Create(Fixture* fixtureA, s32, Fixture* fixtureB, s32, BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(PolygonAndCircleContact));
	return new (mem) PolygonAndCircleContact(fixtureA, fixtureB);
}

void PolygonAndCircleContact::Destroy(Contact* contact, BlockAllocator* allocator)
{
	((PolygonAndCircleContact*)contact)->~PolygonAndCircleContact();
	allocator->Free(contact, sizeof(PolygonAndCircleContact));
}

PolygonAndCircleContact::PolygonAndCircleContact(Fixture* fixtureA, Fixture* fixtureB)
	: Contact(fixtureA, 0, fixtureB, 0)
{
	assert(m_fixtureA->GetType() == Shape::polygon);
	assert(m_fixtureB->GetType() == Shape::circle);
}

void PolygonAndCircleContact::Evaluate(Manifold* manifold, const Transform2D& xfA, const Transform2D& xfB)
{
	CollidePolygonAndCircle(	manifold,
		(PolygonShape*)m_fixtureA->GetShape(), xfA,
		(CircleShape*)m_fixtureB->GetShape(), xfB);
}