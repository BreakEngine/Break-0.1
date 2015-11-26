#include "ChainCircleContact.hpp"
#include "BlockAllocator.hpp"
#include "Fixture.hpp"
#include "ChainShape.hpp"
#include "EdgeShape.hpp"

#include <new>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


Contact* ChainAndCircleContact::Create(Fixture* fixtureA, s32 indexA, Fixture* fixtureB, s32 indexB, BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(ChainAndCircleContact));
	return new (mem) ChainAndCircleContact(fixtureA, indexA, fixtureB, indexB);
}

void ChainAndCircleContact::Destroy(Contact* contact, BlockAllocator* allocator)
{
	((ChainAndCircleContact*)contact)->~ChainAndCircleContact();
	allocator->Free(contact, sizeof(ChainAndCircleContact));
}

ChainAndCircleContact::ChainAndCircleContact(Fixture* fixtureA, s32 indexA, Fixture* fixtureB, s32 indexB)
	: Contact(fixtureA, indexA, fixtureB, indexB)
{
	assert(m_fixtureA->GetType() == Shape::chain);
	assert(m_fixtureB->GetType() == Shape::circle);
}

void ChainAndCircleContact::Evaluate(Manifold* manifold, const Transform2D& xfA, const Transform2D& xfB)
{
	ChainShape* chain = (ChainShape*)m_fixtureA->GetShape();
	EdgeShape edge;
	chain->GetChildEdge(&edge, m_indexA);
	CollideEdgeAndCircle(	manifold, &edge, xfA,
		(CircleShape*)m_fixtureB->GetShape(), xfB);
}

