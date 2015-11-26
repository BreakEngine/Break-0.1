#include "Contact2D.hpp"
#include "CircleContact.hpp"
#include "PolygonCircleContact.hpp"
#include "PolygonContact.hpp"
#include "EdgeCircleContact.hpp"
#include "EdgePolygonContact.hpp"
#include "ChainCircleContact.hpp"
#include "ChainPolygonContact.hpp"
#include "ContactSolver.hpp"

#include "Collision.hpp"
#include "TimeOfImpact.hpp"
#include "Shape.hpp"
#include "BlockAllocator.hpp"
#include "Body2D.hpp"
#include "Fixture.hpp"
#include "World2D.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


ContactRegister Contact::s_registers[Shape::typeCount][Shape::typeCount];
bool Contact::s_initialized = false;

void Contact::InitializeRegisters()
{
	AddType(CircleContact::Create, CircleContact::Destroy, Shape::circle, Shape::circle);
	AddType(PolygonAndCircleContact::Create, PolygonAndCircleContact::Destroy, Shape::polygon, Shape::circle);
	AddType(PolygonContact::Create, PolygonContact::Destroy, Shape::polygon, Shape::polygon);
	AddType(EdgeAndCircleContact::Create, EdgeAndCircleContact::Destroy, Shape::edge, Shape::circle);
	AddType(EdgeAndPolygonContact::Create, EdgeAndPolygonContact::Destroy, Shape::edge, Shape::polygon);
	AddType(ChainAndCircleContact::Create, ChainAndCircleContact::Destroy, Shape::chain, Shape::circle);
	AddType(ChainAndPolygonContact::Create, ChainAndPolygonContact::Destroy, Shape::chain, Shape::polygon);
}

void Contact::AddType(ContactCreateFcn* createFcn, ContactDestroyFcn* destoryFcn,
					  Shape::Type type1, Shape::Type type2)
{
	assert(0 <= type1 && type1 < Shape::typeCount);
	assert(0 <= type2 && type2 < Shape::typeCount);

	s_registers[type1][type2].createFcn = createFcn;
	s_registers[type1][type2].destroyFcn = destoryFcn;
	s_registers[type1][type2].primary = true;

	if (type1 != type2)
	{
		s_registers[type2][type1].createFcn = createFcn;
		s_registers[type2][type1].destroyFcn = destoryFcn;
		s_registers[type2][type1].primary = false;
	}
}

Contact* Contact::Create(Fixture* fixtureA, s32 indexA, Fixture* fixtureB, s32 indexB, BlockAllocator* allocator)
{
	if (s_initialized == false)
	{
		InitializeRegisters();
		s_initialized = true;
	}

	Shape::Type type1 = fixtureA->GetType();
	Shape::Type type2 = fixtureB->GetType();

	assert(0 <= type1 && type1 < Shape::typeCount);
	assert(0 <= type2 && type2 < Shape::typeCount);

	ContactCreateFcn* createFcn = s_registers[type1][type2].createFcn;
	if (createFcn)
	{
		if (s_registers[type1][type2].primary)
		{
			return createFcn(fixtureA, indexA, fixtureB, indexB, allocator);
		}
		else
		{
			return createFcn(fixtureB, indexB, fixtureA, indexA, allocator);
		}
	}
	else
	{
		return NULL;
	}
}

void Contact::Destroy(Contact* contact, BlockAllocator* allocator)
{
	assert(s_initialized == true);

	Fixture* fixtureA = contact->m_fixtureA;
	Fixture* fixtureB = contact->m_fixtureB;

	if (contact->m_manifold.pointCount > 0 &&
		fixtureA->IsSensor() == false &&
		fixtureB->IsSensor() == false)
	{
		fixtureA->GetBody()->SetAwake(true);
		fixtureB->GetBody()->SetAwake(true);
	}

	Shape::Type typeA = fixtureA->GetType();
	Shape::Type typeB = fixtureB->GetType();

	assert(0 <= typeA && typeB < Shape::typeCount);
	assert(0 <= typeA && typeB < Shape::typeCount);

	ContactDestroyFcn* destroyFcn = s_registers[typeA][typeB].destroyFcn;
	destroyFcn(contact, allocator);
}

Contact::Contact(Fixture* fA, s32 indexA, Fixture* fB, s32 indexB)
{
	m_flags = enabledFlag;

	m_fixtureA = fA;
	m_fixtureB = fB;

	m_indexA = indexA;
	m_indexB = indexB;

	m_manifold.pointCount = 0;

	m_prev = NULL;
	m_next = NULL;

	m_nodeA.contact = NULL;
	m_nodeA.prev = NULL;
	m_nodeA.next = NULL;
	m_nodeA.other = NULL;

	m_nodeB.contact = NULL;
	m_nodeB.prev = NULL;
	m_nodeB.next = NULL;
	m_nodeB.other = NULL;

	m_toiCount = 0;

	m_friction = MixFriction(m_fixtureA->m_friction, m_fixtureB->m_friction);
	m_restitution = MixRestitution(m_fixtureA->m_restitution, m_fixtureB->m_restitution);

	m_tangentSpeed = 0.0f;
}

// Update the contact manifold and touching status.
// Note: do not assume the fixture AABBs are overlapping or are valid.
void Contact::Update(ContactListener* listener)
{
	Manifold oldManifold = m_manifold;

	// Re-enable this contact.
	m_flags |= enabledFlag;  ///<<<=================

	bool touching = false;
	bool wasTouching = (m_flags & touchingFlag) == touchingFlag;

	bool sensorA = m_fixtureA->IsSensor();
	bool sensorB = m_fixtureB->IsSensor();
	bool sensor = sensorA || sensorB;

	Body* bodyA = m_fixtureA->GetBody();
	Body* bodyB = m_fixtureB->GetBody();
	const Transform2D& xfA = bodyA->GetTransform2D();
	const Transform2D& xfB = bodyB->GetTransform2D();

	// Is this contact a sensor?
	if (sensor)
	{
		const Shape* shapeA = m_fixtureA->GetShape();
		const Shape* shapeB = m_fixtureB->GetShape();
		touching = TestOverlap(shapeA, m_indexA, shapeB, m_indexB, xfA, xfB);

		// Sensors don't generate manifolds.
		m_manifold.pointCount = 0;
	}
	else
	{
		Evaluate(&m_manifold, xfA, xfB);
		touching = m_manifold.pointCount > 0;

		// Match old contact ids to new contact ids and copy the
		// stored impulses to warm start the solver.
		for (s32 i = 0; i < m_manifold.pointCount; ++i)
		{
			ManifoldPoint* mp2 = m_manifold.points + i;
			mp2->normalImpulse = 0.0f;
			mp2->tangentImpulse = 0.0f;
			ContactID id2 = mp2->id;

			for (s32 j = 0; j < oldManifold.pointCount; ++j)
			{
				ManifoldPoint* mp1 = oldManifold.points + j;

				if (mp1->id.key == id2.key)
				{
					mp2->normalImpulse = mp1->normalImpulse;
					mp2->tangentImpulse = mp1->tangentImpulse;
					break;
				}
			}
		}

		if (touching != wasTouching)
		{
			bodyA->SetAwake(true);
			bodyB->SetAwake(true);
		}
	}

	if (touching)
	{
		m_flags |= touchingFlag;
	}
	else
	{
		m_flags &= ~touchingFlag;
	}

	if (wasTouching == false && touching == true && listener)
	{
		listener->BeginContact(this);  ///<<<-------
	}

	if (wasTouching == true && touching == false && listener)
	{
		listener->EndContact(this);
	}

	if (sensor == false && touching && listener)
	{
		listener->PreSolve(this, &oldManifold);
	}
}
