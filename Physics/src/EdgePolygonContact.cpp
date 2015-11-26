#include "EdgePolygonContact.hpp"
#include "BlockAllocator.hpp"
#include "Fixture.hpp"

#include <new>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;

Contact* EdgeAndPolygonContact::Create(Fixture* fixtureA, s32, Fixture* fixtureB, s32, BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(EdgeAndPolygonContact));
	return new (mem) EdgeAndPolygonContact(fixtureA, fixtureB);
}

void EdgeAndPolygonContact::Destroy(Contact* contact, BlockAllocator* allocator)
{
	((EdgeAndPolygonContact*)contact)->~EdgeAndPolygonContact();
	allocator->Free(contact, sizeof(EdgeAndPolygonContact));
}

EdgeAndPolygonContact::EdgeAndPolygonContact(Fixture* fixtureA, Fixture* fixtureB)
	: Contact(fixtureA, 0, fixtureB, 0)
{
	assert(m_fixtureA->GetType() == Shape::edge);
	assert(m_fixtureB->GetType() == Shape::polygon);
}

void EdgeAndPolygonContact::Evaluate(Manifold* manifold, const Transform2D& xfA, const Transform2D& xfB)
{
	CollideEdgeAndPolygon(	manifold,
		(EdgeShape*)m_fixtureA->GetShape(), xfA,
		(PolygonShape*)m_fixtureB->GetShape(), xfB);
}