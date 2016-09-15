#include "BroadPhase.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;


BroadPhase::BroadPhase()
{
	m_proxyCount = 0;

	m_pairCapacity = 16;
	m_pairCount = 0;
	m_pairBuffer = (Pair*)malloc(m_pairCapacity * sizeof(Pair));

	m_moveCapacity = 16;
	m_moveCount = 0;
	m_moveBuffer = (s32*)malloc(m_moveCapacity * sizeof(s32));
}

BroadPhase::~BroadPhase()
{
	free(m_moveBuffer);
	free(m_pairBuffer);
}

s32 BroadPhase::CreateProxy(const AABB& aabb, void* userData)
{
	s32 proxyId = m_tree.CreateProxy(aabb, userData);
	++m_proxyCount;
	BufferMove(proxyId);
	return proxyId;
}

void BroadPhase::DestroyProxy(s32 proxyId)
{
	UnBufferMove(proxyId);
	--m_proxyCount;
	m_tree.DestroyProxy(proxyId);
}

void BroadPhase::MoveProxy(s32 proxyId, const AABB& aabb, const glm::vec2& displacement)
{
	bool buffer = m_tree.MoveProxy(proxyId, aabb, displacement);
	if (buffer)
	{
		BufferMove(proxyId);
	}
}

void BroadPhase::TouchProxy(s32 proxyId)
{
	BufferMove(proxyId);
}

void BroadPhase::BufferMove(s32 proxyId)
{
	if (m_moveCount == m_moveCapacity)
	{
		s32* oldBuffer = m_moveBuffer;
		m_moveCapacity *= 2;
		m_moveBuffer = (s32*)malloc(m_moveCapacity * sizeof(s32));
        memcpy(m_moveBuffer, oldBuffer, m_moveCount * sizeof(s32));
		free(oldBuffer);
	}

	m_moveBuffer[m_moveCount] = proxyId;
	++m_moveCount;
}

void BroadPhase::UnBufferMove(s32 proxyId)
{
	for (s32 i = 0; i < m_moveCount; ++i)
	{
		if (m_moveBuffer[i] == proxyId)
		{
			m_moveBuffer[i] = nullProxy;
		}
	}
}

// This is called from DynamicTree::Query when we are gathering pairs.
bool BroadPhase::QueryCallback(s32 proxyId)
{
	// A proxy cannot form a pair with itself.
	if (proxyId == m_queryProxyId)
	{
		return true;
	}

	// Grow the pair buffer as needed.
	if (m_pairCount == m_pairCapacity)
	{
		Pair* oldBuffer = m_pairBuffer;
		m_pairCapacity *= 2;
		m_pairBuffer = (Pair*)malloc(m_pairCapacity * sizeof(Pair));
        memcpy(m_pairBuffer, oldBuffer, m_pairCount * sizeof(Pair));
		free(oldBuffer);
	}

	m_pairBuffer[m_pairCount].proxyIdA = glm::min(proxyId, m_queryProxyId);
	m_pairBuffer[m_pairCount].proxyIdB = glm::max(proxyId, m_queryProxyId);
	++m_pairCount;

	return true;
}
