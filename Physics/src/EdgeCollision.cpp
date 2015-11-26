#include "Collision.hpp"
#include "CircleShape.hpp"
#include "EdgeShape.hpp"
#include "PolygonShape.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


// Compute contact points for edge versus circle.
// This accounts for edge connectivity.
void physics::CollideEdgeAndCircle(Manifold* manifold,const EdgeShape* edgeA, const Transform2D& xfA,const CircleShape* circleB, const Transform2D& xfB)
{
	manifold->pointCount = 0;

	// Compute circle in frame of edge
	glm::vec2 Q = MathUtils::MulT(xfA, MathUtils::Mul(xfB, circleB->m_p));

	glm::vec2 A = edgeA->m_vertex1, B = edgeA->m_vertex2;
	glm::vec2 e = B - A;

	// Barycentric coordinates
	real32 u = glm::dot(e, B - Q);
	real32 v = glm::dot(e, Q - A);

	real32 radius = edgeA->m_radius + circleB->m_radius;

	ContactFeature cf;
	cf.indexB = 0;
	cf.typeB = ContactFeature::vertex;

	// Region A
	if (v <= 0.0f)
	{
		glm::vec2 P = A;
		glm::vec2 d = Q - P;
		real32 dd = glm::dot(d, d);
		if (dd > radius * radius)
		{
			return;
		}

		// Is there an edge connected to A?
		if (edgeA->m_hasVertex0)
		{
			glm::vec2 A1 = edgeA->m_vertex0;
			glm::vec2 B1 = A;
			glm::vec2 e1 = B1 - A1;
			real32 u1 = glm::dot(e1, B1 - Q);

			// Is the circle in Region AB of the previous edge?
			if (u1 > 0.0f)
			{
				return;
			}
		}

		cf.indexA = 0;
		cf.typeA = ContactFeature::vertex;
		manifold->pointCount = 1;
		manifold->type = Manifold::circles;
		manifold->localNormal = glm::vec2(0,0);
		manifold->localPoint = P;
		manifold->points[0].id.key = 0;
		manifold->points[0].id.cf = cf;
		manifold->points[0].localPoint = circleB->m_p;
		return;
	}

	// Region B
	if (u <= 0.0f)
	{
		glm::vec2 P = B;
		glm::vec2 d = Q - P;
		real32 dd = glm::dot(d, d);
		if (dd > radius * radius)
		{
			return;
		}

		// Is there an edge connected to B?
		if (edgeA->m_hasVertex3)
		{
			glm::vec2 B2 = edgeA->m_vertex3;
			glm::vec2 A2 = B;
			glm::vec2 e2 =  - A2;
			real32 v2 = glm::dot(e2, Q - A2);

			// Is the circle in Region AB of the next edge?
			if (v2 > 0.0f)
			{
				return;
			}
		}

		cf.indexA = 1;
		cf.typeA = ContactFeature::vertex;
		manifold->pointCount = 1;
		manifold->type = Manifold::circles;
		manifold->localNormal = glm::vec2(0,0);
		manifold->localPoint = P;
		manifold->points[0].id.key = 0;
		manifold->points[0].id.cf = cf;
		manifold->points[0].localPoint = circleB->m_p;
		return;
	}

	// Region AB
	real32 den = glm::dot(e, e);
	assert(den > 0.0f);
	glm::vec2 P = (1.0f / den) * (u * A + v * B);
	glm::vec2 d = Q - P;
	real32 dd = glm::dot(d, d);
	if (dd > radius * radius)
	{
		return;
	}

	glm::vec2 n(-e.y, e.x);
	if (glm::dot(n, Q - A) < 0.0f)
	{
		n  = glm::vec2(-n.x, -n.y);
	}
	n = glm::normalize(n);

	cf.indexA = 0;
	cf.typeA = ContactFeature::face;
	manifold->pointCount = 1;
	manifold->type = Manifold::faceA;
	manifold->localNormal = n;
	manifold->localPoint = A;
	manifold->points[0].id.key = 0;
	manifold->points[0].id.cf = cf;
	manifold->points[0].localPoint = circleB->m_p;
}

// This structure is used to keep track of the best separating axis.
struct BREAK_API EPAxis
{
	enum Type
	{
		unknown,
		edgeA,
		edgeB
	};

	Type type;
	s32 index;
	real32 separation;
};

// This holds polygon B expressed in frame A.
struct BREAK_API TempPolygon
{
	glm::vec2 vertices[maxPolygonVertices];
	glm::vec2 normals[maxPolygonVertices];
	s32 count;
};

// Reference face used for clipping
struct BREAK_API ReferenceFace
{
	s32 i1, i2;

	glm::vec2 v1, v2;

	glm::vec2 normal;

	glm::vec2 sideNormal1;
	real32 sideOffset1;

	glm::vec2 sideNormal2;
	real32 sideOffset2;
};

// This class collides and edge and a polygon, taking into account edge adjacency.
struct BREAK_API EPCollider
{
	void Collide(Manifold* manifold, const EdgeShape* edgeA, const Transform2D& xfA,const PolygonShape* polygonB, const Transform2D& xfB);
	EPAxis ComputeEdgeSeparation();
	EPAxis ComputePolygonSeparation();

	enum VertexType
	{
		isolated,
		concave,
		convex
	};

	TempPolygon m_polygonB;

	Transform2D m_xf;
	glm::vec2 m_centroidB;
	glm::vec2 m_v0, m_v1, m_v2, m_v3;
	glm::vec2 m_normal0, m_normal1, m_normal2;
	glm::vec2 m_normal;
	VertexType m_type1, m_type2;
	glm::vec2 m_lowerLimit, m_upperLimit;
	real32 m_radius;
	bool m_front;
};



// Algorithm:
// 1. Classify v1 and v2
// 2. Classify polygon centroid as front or back
// 3. Flip normal if necessary
// 4. Initialize normal range to [-pi, pi] about face normal
// 5. Adjust normal range according to adjacent edges
// 6. Visit each separating axes, only accept axes within the range
// 7. Return if _any_ axis indicates separation
// 8. Clip
void EPCollider::Collide(Manifold* manifold, const EdgeShape* edgeA, const Transform2D& xfA, const PolygonShape* polygonB, const Transform2D& xfB)
{
	m_xf = MathUtils::MulT(xfA, xfB);

	m_centroidB = MathUtils::Mul(m_xf, polygonB->m_centroid);

	m_v0 = edgeA->m_vertex0;
	m_v1 = edgeA->m_vertex1;
	m_v2 = edgeA->m_vertex2;
	m_v3 = edgeA->m_vertex3;

	bool hasVertex0 = edgeA->m_hasVertex0;
	bool hasVertex3 = edgeA->m_hasVertex3;

	glm::vec2 edge1 = m_v2 - m_v1;
	edge1 = glm::normalize(edge1);
	m_normal1 = glm::vec2(edge1.y, -edge1.x);
	real32 offset1 = glm::dot(m_normal1, m_centroidB - m_v1);
	real32 offset0 = 0.0f, offset2 = 0.0f;
	bool convex1 = false, convex2 = false;

	// Is there a preceding edge?
	if (hasVertex0)
	{
		glm::vec2 edge0 = m_v1 - m_v0;
		edge0 = glm::normalize(edge0);
		m_normal0 = glm::vec2(edge0.y, -edge0.x);
		convex1 = MathUtils::Cross2(edge0, edge1) >= 0.0f;
		offset0 = glm::dot(m_normal0, m_centroidB - m_v0);
	}

	// Is there a following edge?
	if (hasVertex3)
	{
		glm::vec2 edge2 = m_v3 - m_v2;
		edge2 = glm::normalize(edge2);
		m_normal2 = glm::vec2(edge2.y, -edge2.x);
		convex2 = MathUtils::Cross2(edge1, edge2) > 0.0f;
		offset2 = glm::dot(m_normal2, m_centroidB - m_v2);
	}

	// Determine front or back collision. Determine collision normal limits.
	if (hasVertex0 && hasVertex3)
	{
		if (convex1 && convex2)
		{
			m_front = offset0 >= 0.0f || offset1 >= 0.0f || offset2 >= 0.0f;
			if (m_front)
			{
				m_normal = m_normal1;
				m_lowerLimit = m_normal0;
				m_upperLimit = m_normal2;
			}
			else
			{
				m_normal = -m_normal1;
				m_lowerLimit = -m_normal1;
				m_upperLimit = -m_normal1;
			}
		}
		else if (convex1)
		{
			m_front = offset0 >= 0.0f || (offset1 >= 0.0f && offset2 >= 0.0f);
			if (m_front)
			{
				m_normal = m_normal1;
				m_lowerLimit = m_normal0;
				m_upperLimit = m_normal1;
			}
			else
			{
				m_normal = -m_normal1;
				m_lowerLimit = -m_normal2;
				m_upperLimit = -m_normal1;
			}
		}
		else if (convex2)
		{
			m_front = offset2 >= 0.0f || (offset0 >= 0.0f && offset1 >= 0.0f);
			if (m_front)
			{
				m_normal = m_normal1;
				m_lowerLimit = m_normal1;
				m_upperLimit = m_normal2;
			}
			else
			{
				m_normal = -m_normal1;
				m_lowerLimit = -m_normal1;
				m_upperLimit = -m_normal0;
			}
		}
		else
		{
			m_front = offset0 >= 0.0f && offset1 >= 0.0f && offset2 >= 0.0f;
			if (m_front)
			{
				m_normal = m_normal1;
				m_lowerLimit = m_normal1;
				m_upperLimit = m_normal1;
			}
			else
			{
				m_normal = -m_normal1;
				m_lowerLimit = -m_normal2;
				m_upperLimit = -m_normal0;
			}
		}
	}
	else if (hasVertex0)
	{
		if (convex1)
		{
			m_front = offset0 >= 0.0f || offset1 >= 0.0f;
			if (m_front)
			{
				m_normal = m_normal1;
				m_lowerLimit = m_normal0;
				m_upperLimit = -m_normal1;
			}
			else
			{
				m_normal = -m_normal1;
				m_lowerLimit = m_normal1;
				m_upperLimit = -m_normal1;
			}
		}
		else
		{
			m_front = offset0 >= 0.0f && offset1 >= 0.0f;
			if (m_front)
			{
				m_normal = m_normal1;
				m_lowerLimit = m_normal1;
				m_upperLimit = -m_normal1;
			}
			else
			{
				m_normal = -m_normal1;
				m_lowerLimit = m_normal1;
				m_upperLimit = -m_normal0;
			}
		}
	}
	else if (hasVertex3)
	{
		if (convex2)
		{
			m_front = offset1 >= 0.0f || offset2 >= 0.0f;
			if (m_front)
			{
				m_normal = m_normal1;
				m_lowerLimit = -m_normal1;
				m_upperLimit = m_normal2;
			}
			else
			{
				m_normal = -m_normal1;
				m_lowerLimit = -m_normal1;
				m_upperLimit = m_normal1;
			}
		}
		else
		{
			m_front = offset1 >= 0.0f && offset2 >= 0.0f;
			if (m_front)
			{
				m_normal = m_normal1;
				m_lowerLimit = -m_normal1;
				m_upperLimit = m_normal1;
			}
			else
			{
				m_normal = -m_normal1;
				m_lowerLimit = -m_normal2;
				m_upperLimit = m_normal1;
			}
		}		
	}
	else
	{
		m_front = offset1 >= 0.0f;
		if (m_front)
		{
			m_normal = m_normal1;
			m_lowerLimit = -m_normal1;
			m_upperLimit = -m_normal1;
		}
		else
		{
			m_normal = -m_normal1;
			m_lowerLimit = m_normal1;
			m_upperLimit = m_normal1;
		}
	}

	// Get polygonB in frameA
	m_polygonB.count = polygonB->m_count;
	for (s32 i = 0; i < polygonB->m_count; ++i)
	{
		m_polygonB.vertices[i] = MathUtils::Mul(m_xf, polygonB->m_vertices[i]);
		m_polygonB.normals[i] = MathUtils::Mul(m_xf.q, polygonB->m_normals[i]);
	}

	m_radius = 2.0f * polygonRadius;

	manifold->pointCount = 0;

	EPAxis edgeAxis = ComputeEdgeSeparation();

	// If no valid normal can be found than this edge should not collide.
	if (edgeAxis.type == EPAxis::unknown)
	{
		return;
	}

	if (edgeAxis.separation > m_radius)
	{
		return;
	}

	EPAxis polygonAxis = ComputePolygonSeparation();
	if (polygonAxis.type != EPAxis::unknown && polygonAxis.separation > m_radius)
	{
		return;
	}

	// Use hysteresis for jitter reduction.
	const real32 k_relativeTol = 0.98f;
	const real32 k_absoluteTol = 0.001f;

	EPAxis primaryAxis;
	if (polygonAxis.type == EPAxis::unknown)
	{
		primaryAxis = edgeAxis;
	}
	else if (polygonAxis.separation > k_relativeTol * edgeAxis.separation + k_absoluteTol)
	{
		primaryAxis = polygonAxis;
	}
	else
	{
		primaryAxis = edgeAxis;
	}

	ClipVertex ie[2];
	ReferenceFace rf;
	if (primaryAxis.type == EPAxis::edgeA)
	{
		manifold->type = Manifold::faceA;

		// Search for the polygon normal that is most anti-parallel to the edge normal.
		s32 bestIndex = 0;
		real32 bestValue = glm::dot(m_normal, m_polygonB.normals[0]);
		for (s32 i = 1; i < m_polygonB.count; ++i)
		{
			real32 value = glm::dot(m_normal, m_polygonB.normals[i]);
			if (value < bestValue)
			{
				bestValue = value;
				bestIndex = i;
			}
		}

		s32 i1 = bestIndex;
		s32 i2 = i1 + 1 < m_polygonB.count ? i1 + 1 : 0;

		ie[0].v = m_polygonB.vertices[i1];
		ie[0].id.cf.indexA = 0;
		ie[0].id.cf.indexB = static_cast<u8>(i1);
		ie[0].id.cf.typeA = ContactFeature::face;
		ie[0].id.cf.typeB = ContactFeature::vertex;

		ie[1].v = m_polygonB.vertices[i2];
		ie[1].id.cf.indexA = 0;
		ie[1].id.cf.indexB = static_cast<u8>(i2);
		ie[1].id.cf.typeA = ContactFeature::face;
		ie[1].id.cf.typeB = ContactFeature::vertex;

		if (m_front)
		{
			rf.i1 = 0;
			rf.i2 = 1;
			rf.v1 = m_v1;
			rf.v2 = m_v2;
			rf.normal = m_normal1;
		}
		else
		{
			rf.i1 = 1;
			rf.i2 = 0;
			rf.v1 = m_v2;
			rf.v2 = m_v1;
			rf.normal = -m_normal1;
		}		
	}
	else
	{
		manifold->type = Manifold::faceB;

		ie[0].v = m_v1;
		ie[0].id.cf.indexA = 0;
		ie[0].id.cf.indexB = static_cast<u8>(primaryAxis.index);
		ie[0].id.cf.typeA = ContactFeature::vertex;
		ie[0].id.cf.typeB = ContactFeature::face;

		ie[1].v = m_v2;
		ie[1].id.cf.indexA = 0;
		ie[1].id.cf.indexB = static_cast<u8>(primaryAxis.index);		
		ie[1].id.cf.typeA = ContactFeature::vertex;
		ie[1].id.cf.typeB = ContactFeature::face;

		rf.i1 = primaryAxis.index;
		rf.i2 = rf.i1 + 1 < m_polygonB.count ? rf.i1 + 1 : 0;
		rf.v1 = m_polygonB.vertices[rf.i1];
		rf.v2 = m_polygonB.vertices[rf.i2];
		rf.normal = m_polygonB.normals[rf.i1];
	}

	rf.sideNormal1 = glm::vec2(rf.normal.y, -rf.normal.x);
	rf.sideNormal2 = -rf.sideNormal1;
	rf.sideOffset1 = glm::dot(rf.sideNormal1, rf.v1);
	rf.sideOffset2 = glm::dot(rf.sideNormal2, rf.v2);

	// Clip incident edge against extruded edge1 side edges.
	ClipVertex clipPoints1[2];
	ClipVertex clipPoints2[2];
	s32 np;

	// Clip to box side 1
	np = ClipSegmentToLine(clipPoints1, ie, rf.sideNormal1, rf.sideOffset1, rf.i1);

	if (np < maxManifoldPoints)
	{
		return;
	}

	// Clip to negative box side 1
	np = ClipSegmentToLine(clipPoints2, clipPoints1, rf.sideNormal2, rf.sideOffset2, rf.i2);

	if (np < maxManifoldPoints)
	{
		return;
	}

	// Now clipPoints2 contains the clipped points.
	if (primaryAxis.type == EPAxis::edgeA)
	{
		manifold->localNormal = rf.normal;
		manifold->localPoint = rf.v1;
	}
	else
	{
		manifold->localNormal = polygonB->m_normals[rf.i1];
		manifold->localPoint = polygonB->m_vertices[rf.i1];
	}

	s32 pointCount = 0;
	for (s32 i = 0; i < maxManifoldPoints; ++i)
	{
		real32 separation;

		separation = glm::dot(rf.normal, clipPoints2[i].v - rf.v1);

		if (separation <= m_radius)
		{
			ManifoldPoint* cp = manifold->points + pointCount;

			if (primaryAxis.type == EPAxis::edgeA)
			{
				cp->localPoint = MathUtils::MulT(m_xf, clipPoints2[i].v);
				cp->id = clipPoints2[i].id;
			}
			else
			{
				cp->localPoint = clipPoints2[i].v;
				cp->id.cf.typeA = clipPoints2[i].id.cf.typeB;
				cp->id.cf.typeB = clipPoints2[i].id.cf.typeA;
				cp->id.cf.indexA = clipPoints2[i].id.cf.indexB;
				cp->id.cf.indexB = clipPoints2[i].id.cf.indexA;
			}

			++pointCount;
		}
	}

	manifold->pointCount = pointCount;
}

EPAxis EPCollider::ComputeEdgeSeparation()
{
	EPAxis axis;
	axis.type = EPAxis::edgeA;
	axis.index = m_front ? 0 : 1;
	axis.separation = FLT_MAX;

	for (s32 i = 0; i < m_polygonB.count; ++i)
	{
		real32 s = glm::dot(m_normal, m_polygonB.vertices[i] - m_v1);
		if (s < axis.separation)
		{
			axis.separation = s;
		}
	}

	return axis;
}

EPAxis EPCollider::ComputePolygonSeparation()
{
	EPAxis axis;
	axis.type = EPAxis::unknown;
	axis.index = -1;
	axis.separation = -FLT_MAX;

	glm::vec2 perp(-m_normal.y, m_normal.x);

	for (s32 i = 0; i < m_polygonB.count; ++i)
	{
		glm::vec2 n = -m_polygonB.normals[i];

		real32 s1 = glm::dot(n, m_polygonB.vertices[i] - m_v1);
		real32 s2 = glm::dot(n, m_polygonB.vertices[i] - m_v2);
		real32 s = glm::min(s1, s2);

		if (s > m_radius)
		{
			// No collision
			axis.type = EPAxis::edgeB;
			axis.index = i;
			axis.separation = s;
			return axis;
		}

		// Adjacency
		if (glm::dot(n, perp) >= 0.0f)
		{
			if (glm::dot(n - m_upperLimit, m_normal) < -angularSlop)
			{
				continue;
			}
		}
		else
		{
			if (glm::dot(n - m_lowerLimit, m_normal) < -angularSlop)
			{
				continue;
			}
		}

		if (s > axis.separation)
		{
			axis.type = EPAxis::edgeB;
			axis.index = i;
			axis.separation = s;
		}
	}

	return axis;
}

void  physics::CollideEdgeAndPolygon(	Manifold* manifold,const EdgeShape* edgeA, const Transform2D& xfA, const PolygonShape* polygonB, const Transform2D& xfB)
{
	EPCollider collider;
	collider.Collide(manifold, edgeA, xfA, polygonB, xfB);
}
