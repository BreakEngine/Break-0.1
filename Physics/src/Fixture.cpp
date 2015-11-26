#include "Fixture.hpp"
#include "Contact2D.hpp"
#include "World2D.hpp"
#include "CircleShape.hpp"
#include "EdgeShape.hpp"
#include "PolygonShape.hpp"
#include "ChainShape.hpp"
#include "BroadPhase.hpp"
#include "Collision.hpp"
#include "BlockAllocator.hpp"


using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


Fixture::Fixture()
{
	m_userData = NULL;
	m_body = NULL;
	m_next = NULL;
	m_proxies = NULL;
	m_proxyCount = 0;
	m_shape = NULL;
	m_density = 0.0f;
}

void Fixture::Create(BlockAllocator* allocator, Body* body, const FixtureDef* def)
{
	m_userData = def->userData;
	m_friction = def->friction;
	m_restitution = def->restitution;

	m_body = body;
	m_next = NULL;

	m_filter = def->filter;

	m_isSensor = def->isSensor;

	m_shape = def->shape->Clone(allocator);

	// Reserve proxy space
	s32 childCount = m_shape->GetChildCount();
	m_proxies = (FixtureProxy*)allocator->Allocate(childCount * sizeof(FixtureProxy));
	for (s32 i = 0; i < childCount; ++i)
	{
		m_proxies[i].fixture = NULL;
		m_proxies[i].proxyId = BroadPhase::nullProxy;
	}
	m_proxyCount = 0;

	m_density = def->density;
}

void Fixture::Destroy(BlockAllocator* allocator)
{
	// The proxies must be destroyed before calling this.
	assert(m_proxyCount == 0);

	// Free the proxy array.
	s32 childCount = m_shape->GetChildCount();
	allocator->Free(m_proxies, childCount * sizeof(FixtureProxy));
	m_proxies = NULL;

	// Free the child shape.
	switch (m_shape->m_type)
	{
	case Shape::circle:
		{
			CircleShape* s = (CircleShape*)m_shape;
			s->~CircleShape();
			allocator->Free(s, sizeof(CircleShape));
		}
		break;

	case Shape::edge:
		{
			EdgeShape* s = (EdgeShape*)m_shape;
			s->~EdgeShape();
			allocator->Free(s, sizeof(EdgeShape));
		}
		break;

	case Shape::polygon:
		{
			PolygonShape* s = (PolygonShape*)m_shape;
			s->~PolygonShape();
			allocator->Free(s, sizeof(PolygonShape));
		}
		break;

	case Shape::chain:
		{
			ChainShape* s = (ChainShape*)m_shape;
			s->~ChainShape();
			allocator->Free(s, sizeof(ChainShape));
		}
		break;

	default:
		assert(false);
		break;
	}

	m_shape = NULL;
}

void Fixture::CreateProxies(BroadPhase* broadPhase, const Transform2D& xf)
{
	assert(m_proxyCount == 0);

	// Create proxies in the broad-phase.
	m_proxyCount = m_shape->GetChildCount();

	for (s32 i = 0; i < m_proxyCount; ++i)
	{
		FixtureProxy* proxy = m_proxies + i;
		m_shape->ComputeAABB(&proxy->aabb, xf, i);
		proxy->proxyId = broadPhase->CreateProxy(proxy->aabb, proxy);
		proxy->fixture = this;
		proxy->childIndex = i;
	}
}

void Fixture::DestroyProxies(BroadPhase* broadPhase)
{
	// Destroy proxies in the broad-phase.
	for (s32 i = 0; i < m_proxyCount; ++i)
	{
		FixtureProxy* proxy = m_proxies + i;
		broadPhase->DestroyProxy(proxy->proxyId);
		proxy->proxyId = BroadPhase::nullProxy;
	}

	m_proxyCount = 0;
}

void Fixture::Synchronize(BroadPhase* broadPhase, const Transform2D& Transform2D1, const Transform2D& Transform2D2)
{
	if (m_proxyCount == 0)
	{	
		return;
	}

	for (s32 i = 0; i < m_proxyCount; ++i)
	{
		FixtureProxy* proxy = m_proxies + i;

		// Compute an AABB that covers the swept shape (may miss some rotation effect).
		AABB aabb1, aab;
		m_shape->ComputeAABB(&aabb1, Transform2D1, proxy->childIndex);
		m_shape->ComputeAABB(&aab, Transform2D2, proxy->childIndex);

		proxy->aabb.Combine(aabb1, aab);

		glm::vec2 displacement = Transform2D2.p - Transform2D1.p;

		broadPhase->MoveProxy(proxy->proxyId, proxy->aabb, displacement);
	}
}

void Fixture::SetFilterData(const Filter& filter)
{
	m_filter = filter;

	Refilter();
}

void Fixture::Refilter()
{
	if (m_body == NULL)
	{
		return;
	}

	// Flag associated contacts for filtering.
	ContactEdge* edge = m_body->GetContactList();
	while (edge)
	{
		Contact* contact = edge->contact;
		Fixture* fixtureA = contact->GetFixtureA();
		Fixture* fixtureB = contact->GetFixtureB();
		if (fixtureA == this || fixtureB == this)
		{
			contact->FlagForFiltering();
		}

		edge = edge->next;
	}

	World* world = m_body->GetWorld();

	if (world == NULL)
	{
		return;
	}

	// Touch each proxy so that new pairs may be created
	BroadPhase* broadPhase = &world->m_contactManager.m_broadPhase;
	for (s32 i = 0; i < m_proxyCount; ++i)
	{
		broadPhase->TouchProxy(m_proxies[i].proxyId);
	}
}

void Fixture::SetSensor(bool sensor)
{
	if (sensor != m_isSensor)
	{
		m_body->SetAwake(true);
		m_isSensor = sensor;
	}
}

void Fixture::Dump(s32 bodyIndex)
{
	printf("    FixtureDef fd;\n");
	printf("    fd.friction = %.15lef;\n", m_friction);
	printf("    fd.restitution = %.15lef;\n", m_restitution);
	printf("    fd.density = %.15lef;\n", m_density);
	printf("    fd.isSensor = bool(%d);\n", m_isSensor);
	printf("    fd.filter.categoryBits = uint16(%d);\n", m_filter.categoryBits);
	printf("    fd.filter.maskBits = uint16(%d);\n", m_filter.maskBits);
	printf("    fd.filter.groupIndex = int16(%d);\n", m_filter.groupIndex);

	switch (m_shape->m_type)
	{
	case Shape::circle:
		{
			CircleShape* s = (CircleShape*)m_shape;
			printf("    CircleShape shape;\n");
			printf("    shape.m_radius = %.15lef;\n", s->m_radius);
			printf("    shape.m_p.Set(%.15lef, %.15lef);\n", s->m_p.x, s->m_p.y);
		}
		break;

	case Shape::edge:
		{
			EdgeShape* s = (EdgeShape*)m_shape;
			printf("    EdgeShape shape;\n");
			printf("    shape.m_radius = %.15lef;\n", s->m_radius);
			printf("    shape.m_vertex0.Set(%.15lef, %.15lef);\n", s->m_vertex0.x, s->m_vertex0.y);
			printf("    shape.m_vertex1.Set(%.15lef, %.15lef);\n", s->m_vertex1.x, s->m_vertex1.y);
			printf("    shape.m_vertex2.Set(%.15lef, %.15lef);\n", s->m_vertex2.x, s->m_vertex2.y);
			printf("    shape.m_vertex3.Set(%.15lef, %.15lef);\n", s->m_vertex3.x, s->m_vertex3.y);
			printf("    shape.m_hasVertex0 = bool(%d);\n", s->m_hasVertex0);
			printf("    shape.m_hasVertex3 = bool(%d);\n", s->m_hasVertex3);
		}
		break;

	case Shape::polygon:
		{
			PolygonShape* s = (PolygonShape*)m_shape;
			printf("    PolygonShape shape;\n");
			printf("    glm::vec2 vs[%d];\n", maxPolygonVertices);
			for (s32 i = 0; i < s->m_count; ++i)
			{
				printf("    vs[%d].Set(%.15lef, %.15lef);\n", i, s->m_vertices[i].x, s->m_vertices[i].y);
			}
			printf("    shape.Set(vs, %d);\n", s->m_count);
		}
		break;

	case Shape::chain:
		{
			ChainShape* s = (ChainShape*)m_shape;
			printf("    ChainShape shape;\n");
			printf("    glm::vec2 vs[%d];\n", s->m_count);
			for (s32 i = 0; i < s->m_count; ++i)
			{
				printf("    vs[%d].Set(%.15lef, %.15lef);\n", i, s->m_vertices[i].x, s->m_vertices[i].y);
			}
			printf("    shape.CreateChain(vs, %d);\n", s->m_count);
			printf("    shape.m_prevVertex.Set(%.15lef, %.15lef);\n", s->m_prevVertex.x, s->m_prevVertex.y);
			printf("    shape.m_nextVertex.Set(%.15lef, %.15lef);\n", s->m_nextVertex.x, s->m_nextVertex.y);
			printf("    shape.m_hasPrevVertex = bool(%d);\n", s->m_hasPrevVertex);
			printf("    shape.m_hasNextVertex = bool(%d);\n", s->m_hasNextVertex);
		}
		break;

	default:
		return;
	}

	printf("\n");
	printf("    fd.shape = &shape;\n");
	printf("\n");
	printf("    bodies[%d]->CreateFixture(&fd);\n", bodyIndex);
}
