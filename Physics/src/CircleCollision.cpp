#include "Collision.hpp"
#include "CircleShape.hpp"
#include "PolygonShape.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;


void Physics::CollideCircles(Manifold* manifold,const CircleShape* circleA, const Transform2D& xfA,const CircleShape* circleB, const Transform2D& xfB)
{
	manifold->pointCount = 0;

	glm::vec2 pA = Transform2D::Mul(xfA, circleA->m_p);
	glm::vec2 pB = Transform2D::Mul(xfB, circleB->m_p);

	glm::vec2 d = pB - pA;
	real32 distSqr = glm::dot(d, d);
	real32 rA = circleA->m_radius, rB = circleB->m_radius;
	real32 radius = rA + rB;
	if (distSqr > radius * radius)
	{
		return;
	}

	manifold->type = Manifold::circles;
	manifold->localPoint = circleA->m_p;
	manifold->localNormal = glm::vec2(0,0);
	manifold->pointCount = 1;

	manifold->points[0].localPoint = circleB->m_p;
	manifold->points[0].id.key = 0;
}

void Physics::CollidePolygonAndCircle(Manifold* manifold,const PolygonShape* polygonA, const Transform2D& xfA,const CircleShape* circleB, const Transform2D& xfB)
{
	manifold->pointCount = 0;

	// Compute circle position in the frame of the polygon.
	glm::vec2 c = Transform2D::Mul(xfB, circleB->m_p);
	glm::vec2 cLocal = Transform2D::MulT(xfA, c);

	// Find the min separating edge.
	s32 normalIndex = 0;
	real32 separation = -FLT_MAX;
	real32 radius = polygonA->m_radius + circleB->m_radius;
	s32 vertexCount = polygonA->m_count;
	const glm::vec2* vertices = polygonA->m_vertices;
	const glm::vec2* normals = polygonA->m_normals;

	for (s32 i = 0; i < vertexCount; ++i)
	{
		real32 s = glm::dot(normals[i], cLocal - vertices[i]);

		if (s > radius)
		{
			// Early out.
			return;
		}

		if (s > separation)
		{
			separation = s;
			normalIndex = i;
		}
	}

	// Vertices that subtend the incident face.
	s32 vertIndex1 = normalIndex;
	s32 vertIndex2 = vertIndex1 + 1 < vertexCount ? vertIndex1 + 1 : 0;
	glm::vec2 v1 = vertices[vertIndex1];
	glm::vec2 v2 = vertices[vertIndex2];

	// If the center is inside the polygon ...
	if (separation < FLT_EPSILON)
	{
		manifold->pointCount = 1;
		manifold->type = Manifold::faceA;
		manifold->localNormal = normals[normalIndex];
		manifold->localPoint = 0.5f * (v1 + v2);
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
		return;
	}

	// Compute barycentric coordinates   <<-------need to know
	real32 u1 = glm::dot(cLocal - v1, v2 - v1);
	real32 u2 = glm::dot(cLocal - v2, v1 - v2);
	if (u1 <= 0.0f)
	{
		if (MathUtils::DistanceSquared(cLocal, v1) > radius * radius)
		{
			return;
		}

		manifold->pointCount = 1;
		manifold->type = Manifold::faceA;
		manifold->localNormal = cLocal - v1;
		manifold->localNormal = glm::normalize(manifold->localNormal);
		manifold->localPoint = v1;
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
	}
	else if (u2 <= 0.0f)
	{
		if (MathUtils::DistanceSquared(cLocal, v2) > radius * radius)
		{
			return;
		}

		manifold->pointCount = 1;
		manifold->type = Manifold::faceA;
		manifold->localNormal = cLocal - v2;
		manifold->localNormal = glm::normalize(manifold->localNormal);
		manifold->localPoint = v2;
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
	}
	else
	{
		glm::vec2 faceCenter = 0.5f * (v1 + v2);
		real32 separation = glm::dot(cLocal - faceCenter, normals[vertIndex1]);
		if (separation > radius)
		{
			return;
		}

		manifold->pointCount = 1;
		manifold->type = Manifold::faceA;
		manifold->localNormal = normals[vertIndex1];
		manifold->localPoint = faceCenter;
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
	}
}
