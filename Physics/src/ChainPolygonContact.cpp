#include "ChainPolygonContact.hpp"
#include "BlockAllocator.hpp"
#include "Fixture.hpp"
#include "ChainShape.hpp"
#include "EdgeShape.hpp"
#include <new>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;

Contact* ChainAndPolygonContact::Create(Fixture* fixtureA, s32 indexA, Fixture* fixtureB, s32 indexB, BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(ChainAndPolygonContact));
	return new (mem) ChainAndPolygonContact(fixtureA, indexA, fixtureB, indexB);
}

void ChainAndPolygonContact::Destroy(Contact* contact, BlockAllocator* allocator)
{
	((ChainAndPolygonContact*)contact)->~ChainAndPolygonContact();
	allocator->Free(contact, sizeof(ChainAndPolygonContact));
}

ChainAndPolygonContact::ChainAndPolygonContact(Fixture* fixtureA, s32 indexA, Fixture* fixtureB, s32 indexB)
	: Contact(fixtureA, indexA, fixtureB, indexB)
{
	assert(m_fixtureA->GetType() == Shape::chain);
	assert(m_fixtureB->GetType() == Shape::polygon);
}

void ChainAndPolygonContact::Evaluate(Manifold* manifold, const Transform2D& xfA, const Transform2D& xfB)
{
	ChainShape* chain = (ChainShape*)m_fixtureA->GetShape();
	EdgeShape edge;
	chain->GetChildEdge(&edge, m_indexA);
	CollideEdgeAndPolygon(	manifold, &edge, xfA,
		(PolygonShape*)m_fixtureB->GetShape(), xfB);
}
