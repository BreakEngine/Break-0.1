#include "EdgeCircleContact.hpp"
#include "BlockAllocator.hpp"
#include "Fixture.hpp"

#include <new>


using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;


Contact* EdgeAndCircleContact::Create(Fixture* fixtureA, s32, Fixture* fixtureB, s32, BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(EdgeAndCircleContact));
	return new (mem) EdgeAndCircleContact(fixtureA, fixtureB);
}

void EdgeAndCircleContact::Destroy(Contact* contact, BlockAllocator* allocator)
{
	((EdgeAndCircleContact*)contact)->~EdgeAndCircleContact();
	allocator->Free(contact, sizeof(EdgeAndCircleContact));
}

EdgeAndCircleContact::EdgeAndCircleContact(Fixture* fixtureA, Fixture* fixtureB)
	: Contact(fixtureA, 0, fixtureB, 0)
{
	assert(m_fixtureA->GetType() == Shape::edge);
	assert(m_fixtureB->GetType() == Shape::circle);
}

void EdgeAndCircleContact::Evaluate(Manifold* manifold, const Transform2D& xfA, const Transform2D& xfB)
{
	CollideEdgeAndCircle(	manifold,
		(EdgeShape*)m_fixtureA->GetShape(), xfA,
		(CircleShape*)m_fixtureB->GetShape(), xfB);
}
