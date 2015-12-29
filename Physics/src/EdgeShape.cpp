#include "EdgeShape.hpp"
#include <new>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;


void EdgeShape::Set(const glm::vec2& v1, const glm::vec2& v2)
{
	m_vertex1 = v1;
	m_vertex2 = v2;
	m_hasVertex0 = false;
	m_hasVertex3 = false;
}

Shape* EdgeShape::Clone(BlockAllocator* allocator) const
{
	void* mem = allocator->Allocate(sizeof(EdgeShape));
	EdgeShape* clone = new (mem) EdgeShape;
	*clone = *this;
	return clone;
}

s32 EdgeShape::GetChildCount() const
{
	return 1;
}

bool EdgeShape::TestPoint(const Transform2D& xf, const glm::vec2& p) const
{
	NOT_USED(xf);
	NOT_USED(p);
	return false;
}

// p = p1 + t * d
// v = v1 + s * e
// p1 + t * d = v1 + s * e
// s * e - t * d = p1 - v1
bool EdgeShape::RayCast(RayCastOutput* output, const RayCastInput& input,const Transform2D& xf, s32 childIndex) const
{
	NOT_USED(childIndex);

	// Put the ray into the edge's frame of reference.
	glm::vec2 p1 = Rotation2D::MulT(xf.q, input.p1 - xf.p);
	glm::vec2 p2 = Rotation2D::MulT(xf.q, input.p2 - xf.p);
	glm::vec2 d = p2 - p1;

	glm::vec2 v1 = m_vertex1;
	glm::vec2 v2 = m_vertex2;
	glm::vec2 e = v2 - v1;
	glm::vec2 normal(e.y, -e.x);
	normal = glm::normalize(normal);

	// q = p1 + t * d
	// dot(normal, q - v1) = 0
	// dot(normal, p1 - v1) + t * dot(normal, d) = 0
	real32 numerator = glm::dot(normal, v1 - p1);
	real32 denominator = glm::dot(normal, d);

	if (denominator == 0.0f)
	{
		return false;
	}

	real32 t = numerator / denominator;
	if (t < 0.0f || input.maxFraction < t)
	{
		return false;
	}

	glm::vec2 q = p1 + t * d;

	// q = v1 + s * r
	// s = dot(q - v1, r) / dot(r, r)
	glm::vec2 r = v2 - v1;
	real32 rr = glm::dot(r, r);
	if (rr == 0.0f)
	{
		return false;
	}

	real32 s = glm::dot(q - v1, r) / rr;
	if (s < 0.0f || 1.0f < s)
	{
		return false;
	}

	output->fraction = t;
	if (numerator > 0.0f)
	{
		output->normal = -Rotation2D::Mul(xf.q, normal);
	}
	else
	{
		output->normal = Rotation2D::Mul(xf.q, normal);
	}
	return true;
}

void EdgeShape::ComputeAABB(AABB* aabb, const Transform2D& xf, s32 childIndex) const
{
	NOT_USED(childIndex);

	glm::vec2 v1 = Transform2D::Mul(xf, m_vertex1);
	glm::vec2 v2 = Transform2D::Mul(xf, m_vertex2);

	glm::vec2 lower = glm::min(v1, v2);
	glm::vec2 upper = glm::max(v1, v2);

	glm::vec2 r(m_radius, m_radius);
	aabb->lowerBound = lower - r;
	aabb->upperBound = upper + r;
}

void EdgeShape::ComputeMass(MassData* massData, real32 density) const
{
	NOT_USED(density);

	massData->mass = 0.0f;
	massData->center = 0.5f * (m_vertex1 + m_vertex2);
	massData->I = 0.0f;
}