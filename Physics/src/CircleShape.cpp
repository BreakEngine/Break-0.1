#include "CircleShape.hpp"
#include <new>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


Shape* CircleShape::Clone(BlockAllocator* allocator) const
{
	void* mem = allocator->Allocate(sizeof(CircleShape));
	CircleShape* clone = new (mem) CircleShape;
	*clone = *this;
	return clone;
}

s32 CircleShape::GetChildCount() const
{
	return 1;
}

bool CircleShape::TestPoint(const Transform2D& Transform2D, const glm::vec2& p) const
{
	glm::vec2 center = Transform2D.p + MathUtils::Mul(Transform2D.q, m_p);
	glm::vec2 d = p - center;
	return glm::dot(d, d) <= m_radius * m_radius;
}

// Collision Detection in Interactive 3D Environments by Gino van den Bergen
// From Section 3.1.2
// x = s + a * r
// norm(x) = radius
bool CircleShape::RayCast(RayCastOutput* output, const RayCastInput& input,const Transform2D& Transform2D, s32 childIndex) const
{
	NOT_USED(childIndex);

	glm::vec2 position = Transform2D.p + MathUtils::Mul(Transform2D.q, m_p);
	glm::vec2 s = input.p1 - position;
	real32 b = glm::dot(s, s) - m_radius * m_radius;

	// Solve quadratic equation.
	glm::vec2 r = input.p2 - input.p1;
	real32 c =  glm::dot(s, r);
	real32 rr = glm::dot(r, r);
	real32 sigma = c * c - rr * b;

	// Check for negative discriminant and short segment.
	if (sigma < 0.0f || rr < FLT_EPSILON)
	{
		return false;
	}

	// Find the point of intersection of the line with the circle.
	real32 a = -(c + sqrtf(sigma));

	// Is the intersection point on the segment?
	if (0.0f <= a && a <= input.maxFraction * rr)
	{
		a /= rr;
		output->fraction = a;
		output->normal = s + a * r;
		output->normal = glm::normalize(output->normal);
		return true;
	}

	return false;
}

void CircleShape::ComputeAABB(AABB* aabb, const Transform2D& Transform2D, s32 childIndex) const
{
	NOT_USED(childIndex);

	glm::vec2 p = Transform2D.p + MathUtils::Mul(Transform2D.q, m_p);
	aabb->lowerBound = glm::vec2(p.x - m_radius, p.y - m_radius);
	aabb->upperBound = glm::vec2(p.x + m_radius, p.y + m_radius);
}

void CircleShape::ComputeMass(MassData* massData, real32 density) const
{
	massData->mass = density * glm::pi<float>() * m_radius * m_radius;
	massData->center = m_p;

	// inertia about the local origin
	massData->I = massData->mass * (0.5f * m_radius * m_radius + glm::dot(m_p, m_p));
}
