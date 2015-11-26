#include "Collision.hpp"
#include "Distance.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;

void WorldManifold::Initialize(const Manifold* manifold,const Transform2D& xfA, real32 radiusA, const Transform2D& xfB, real32 radiusB)
{
	if (manifold->pointCount == 0)
	{
		return;
	}

	switch (manifold->type)
	{
	case Manifold::circles:
		{
			normal = glm::vec2(1.0f, 0.0f);
			glm::vec2 pointA = MathUtils::Mul(xfA, manifold->localPoint);
			glm::vec2 pointB = MathUtils::Mul(xfB, manifold->points[0].localPoint);
			if (MathUtils::DistanceSquared(pointA, pointB) > FLT_EPSILON * FLT_EPSILON)
			{
				normal = pointB - pointA;
				normal = glm::normalize(normal);
			}

			glm::vec2 cA = pointA + radiusA * normal;
			glm::vec2 cB = pointB - radiusB * normal;
			points[0] = 0.5f * (cA + cB);
			separations[0] = glm::dot(cB - cA, normal);
		}
		break;

	case Manifold::faceA:
		{
			normal = MathUtils::Mul(xfA.q, manifold->localNormal);
			glm::vec2 planePoint = MathUtils::Mul(xfA, manifold->localPoint);

			for (s32 i = 0; i < manifold->pointCount; ++i)
			{
				glm::vec2 clipPoint = MathUtils::Mul(xfB, manifold->points[i].localPoint);
				glm::vec2 cA = clipPoint + (radiusA - glm::dot(clipPoint - planePoint, normal)) * normal;
				glm::vec2 cB = clipPoint - radiusB * normal;
				points[i] = 0.5f * (cA + cB);
				separations[i] = glm::dot(cB - cA, normal);
			}
		}
		break;

	case Manifold::faceB:
		{
			normal = MathUtils::Mul(xfB.q, manifold->localNormal);
			glm::vec2 planePoint = MathUtils::Mul(xfB, manifold->localPoint);

			for (s32 i = 0; i < manifold->pointCount; ++i)
			{
				glm::vec2 clipPoint = MathUtils::Mul(xfA, manifold->points[i].localPoint);
				glm::vec2 cB = clipPoint + (radiusB - glm::dot(clipPoint - planePoint, normal)) * normal;
				glm::vec2 cA = clipPoint - radiusA * normal;
				points[i] = 0.5f * (cA + cB);
				separations[i] = glm::dot(cA - cB, normal);
			}

			// Ensure normal points from A to B.
			normal = -normal;
		}
		break;
	}
}


void physics::GetPointStates(PointState state1[maxManifoldPoints], PointState state2[maxManifoldPoints], const Manifold* manifold1, const Manifold* manifold2)
{
	for (s32 i = 0; i < maxManifoldPoints; ++i)
	{
		state1[i] = _nullState;
		state2[i] = _nullState;
	}

	// Detect persists and removes.
	for (s32 i = 0; i < manifold1->pointCount; ++i)
	{
		ContactID id = manifold1->points[i].id;

		state1[i] = _removeState;

		for (s32 j = 0; j < manifold2->pointCount; ++j)
		{
			if (manifold2->points[j].id.key == id.key)
			{
				state1[i] = _persistState;
				break;
			}
		}
	}

	// Detect persists and adds.
	for (s32 i = 0; i < manifold2->pointCount; ++i)
	{
		ContactID id = manifold2->points[i].id;

		state2[i] = _addState;

		for (s32 j = 0; j < manifold1->pointCount; ++j)
		{
			if (manifold1->points[j].id.key == id.key)
			{
				state2[i] = _persistState;
				break;
			}
		}
	}
}


// From Real-time Collision Detection, p179.
///Ray Cast concepts..
bool AABB::RayCast(RayCastOutput* output, const RayCastInput& input) const
{
	real32 tmin = -FLT_MAX;
	real32 tmax = FLT_MAX;

	glm::vec2 p = input.p1;
	glm::vec2 d = input.p2 - input.p1;
	glm::vec2 absD = glm::abs(d);

	glm::vec2 normal;

	for (s32 i = 0; i < 2; ++i)
	{
		if (absD[i] < FLT_EPSILON)
		{
			// Parallel.
			if (p[i] < lowerBound[i] || upperBound[i] < p[i])
			{
				return false;
			}
		}
		else
		{
			real32 inv_d = 1.0f / d[i];
			real32 t1 = (lowerBound[i] - p[i]) * inv_d;
			real32 t2 = (upperBound[i] - p[i]) * inv_d;

			// Sign of the normal vector.
			real32 s = -1.0f;

			if (t1 > t2)
			{
				MathUtils::Swap(t1, t2);
				s = 1.0f;
			}

			// Push the min up
			if (t1 > tmin)
			{
				normal = glm::vec2(0,0);
				normal[i] = s;
				tmin = t1;
			}

			// Pull the max down
			tmax = glm::min(tmax, t2);

			if (tmin > tmax)
			{
				return false;
			}
		}
	}

	// Does the ray start inside the box?
	// Does the ray intersect beyond the max fraction?
	if (tmin < 0.0f || input.maxFraction < tmin)
	{
		return false;
	}

	// Intersection.
	output->fraction = tmin;
	output->normal = normal;
	return true;
}



// Sutherland-Hodgman clipping algorithm..
s32 physics::ClipSegmentToLine(ClipVertex vOut[2], const ClipVertex vIn[2], const glm::vec2& normal, real32 offset, s32 vertexIndexA)
{
	// Start with no output points
	s32 numOut = 0;

	// Calculate the distance of end points to the line
	real32 distance0 = glm::dot(normal, vIn[0].v) - offset;
	real32 distance1 = glm::dot(normal, vIn[1].v) - offset;

	// If the points are behind the plane
	if (distance0 <= 0.0f) vOut[numOut++] = vIn[0];
	if (distance1 <= 0.0f) vOut[numOut++] = vIn[1];

	// If the points are on different sides of the plane
	if (distance0 * distance1 < 0.0f)
	{
		// Find intersection point of edge and plane
		real32 interp = distance0 / (distance0 - distance1);
		vOut[numOut].v = vIn[0].v + interp * (vIn[1].v - vIn[0].v);

		// VertexA is hitting edgeB.
		vOut[numOut].id.cf.indexA = static_cast<u8>(vertexIndexA);
		vOut[numOut].id.cf.indexB = vIn[0].id.cf.indexB;
		vOut[numOut].id.cf.typeA = ContactFeature::vertex;
		vOut[numOut].id.cf.typeB = ContactFeature::face;
		++numOut;
	}

	return numOut;
}



//testing overlaping by calculating distance between two polygons using GJK algorithm..
bool physics::TestOverlap(const Shape* shapeA, s32 indexA,const Shape* shapeB, s32 indexB,const Transform2D& xfA, const Transform2D& xfB)
{
	DistanceInput input;
	input.proxyA.Set(shapeA, indexA);
	input.proxyB.Set(shapeB, indexB);
	input.Transform2DA = xfA;
	input.Transform2DB = xfB;
	input.useRadii = true;

	SimplexCache cache;
	cache.count = 0;

	DistanceOutput output;

	Distance(&output, &cache, &input);

	return output.distance < 10.0f * FLT_EPSILON;
}