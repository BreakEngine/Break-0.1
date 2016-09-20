#include "Collision.hpp"
#include "PolygonShape.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;

// Find the max separation between poly1 and poly2 using edge normals from poly1.
static real32 FindMaxSeparation(s32* edgeIndex,const PolygonShape* poly1, const Transform2D& xf1,const PolygonShape* poly2, const Transform2D& xf2)
{
	s32 count1 = poly1->m_count;
	s32 count2 = poly2->m_count;
	const glm::vec2* n1s = poly1->m_normals;
	const glm::vec2* v1s = poly1->m_vertices;
	const glm::vec2* v2s = poly2->m_vertices;
	Transform2D xf = Transform2D::MulT(xf2, xf1);

	s32 bestIndex = 0;
	real32 maxSeparation = -FLT_MAX;  //<<<---- using of maxfloat
	for (s32 i = 0; i < count1; ++i)
	{
		// Get poly1 normal in frame2.
		glm::vec2 n = Rotation2D::Mul(xf.q, n1s[i]);
		glm::vec2 v1 = Transform2D::Mul(xf, v1s[i]);

		// Find deepest point for normal i.
		real32 si = FLT_MAX;
		for (s32 j = 0; j < count2; ++j)
		{
			real32 sij = glm::dot(n, v2s[j] - v1);
			if (sij < si)
			{
				si = sij;
			}
		}

		if (si > maxSeparation)
		{
			maxSeparation = si;
			bestIndex = i;
		}
	}

	*edgeIndex = bestIndex;
	return maxSeparation;
}

static void FindIncidentEdge(ClipVertex c[2],const PolygonShape* poly1, const Transform2D& xf1, s32 edge1,const PolygonShape* poly2, const Transform2D& xf2)
{
	const glm::vec2* normals1 = poly1->m_normals;

	s32 count2 = poly2->m_count;
	const glm::vec2* vertices2 = poly2->m_vertices;
	const glm::vec2* normals2 = poly2->m_normals;

	assert(0 <= edge1 && edge1 < poly1->m_count);

	// Get the normal of the reference edge in poly2's frame.
	glm::vec2 normal1 = Rotation2D::MulT(xf2.q, Rotation2D::Mul(xf1.q, normals1[edge1]));

	// Find the incident edge on poly2.
	s32 index = 0;
	real32 minDot = FLT_MAX;
	for (s32 i = 0; i < count2; ++i)
	{
		real32 dot = glm::dot(normal1, normals2[i]);
		if (dot < minDot)
		{
			minDot = dot;
			index = i;
		}
	}

	// Build the clip vertices for the incident edge.
	s32 i1 = index;
	s32 i2 = i1 + 1 < count2 ? i1 + 1 : 0;

	c[0].v = Transform2D::Mul(xf2, vertices2[i1]);
	c[0].id.cf.indexA = (u8)edge1;
	c[0].id.cf.indexB = (u8)i1;
	c[0].id.cf.typeA = ContactFeature::face;
	c[0].id.cf.typeB = ContactFeature::vertex;

	c[1].v = Transform2D::Mul(xf2, vertices2[i2]);
	c[1].id.cf.indexA = (u8)edge1;
	c[1].id.cf.indexB = (u8)i2;
	c[1].id.cf.typeA = ContactFeature::face;
	c[1].id.cf.typeB = ContactFeature::vertex;
}

// Find edge normal of max separation on A - return if separating axis is found
// Find edge normal of max separation on B - return if separation axis is found
// Choose reference edge as min(minA, minB)
// Find incident edge
// Clip

// The normal points from 1 to 2
void Physics::CollidePolygons(Manifold* manifold,const PolygonShape* polyA, const Transform2D& xfA,const PolygonShape* polyB, const Transform2D& xfB)
{
	manifold->pointCount = 0;
	real32 totalRadius = polyA->m_radius + polyB->m_radius;

	s32 edgeA = 0;
	real32 separationA = FindMaxSeparation(&edgeA, polyA, xfA, polyB, xfB);
	if (separationA > totalRadius)
		return;

	s32 edgeB = 0;
	real32 separationB = FindMaxSeparation(&edgeB, polyB, xfB, polyA, xfA);
	if (separationB > totalRadius)
		return;

	const PolygonShape* poly1;	// reference polygon
	const PolygonShape* poly2;	// incident polygon
	Transform2D xf1, xf2;
	s32 edge1;					// reference edge
	u8 flip;
	const real32 k_tol = 0.1f * linearSlop;

	if (separationB > separationA + k_tol)
	{
		poly1 = polyB;
		poly2 = polyA;
		xf1 = xfB;
		xf2 = xfA;
		edge1 = edgeB;
		manifold->type = Manifold::faceB;
		flip = 1;
	}
	else
	{
		poly1 = polyA;
		poly2 = polyB;
		xf1 = xfA;
		xf2 = xfB;
		edge1 = edgeA;
		manifold->type = Manifold::faceA;
		flip = 0;
	}

	ClipVertex incidentEdge[2];
	FindIncidentEdge(incidentEdge, poly1, xf1, edge1, poly2, xf2);

	s32 count1 = poly1->m_count;
	const glm::vec2* vertices1 = poly1->m_vertices;

	s32 iv1 = edge1;
	s32 iv2 = edge1 + 1 < count1 ? edge1 + 1 : 0;

	glm::vec2 v11 = vertices1[iv1];
	glm::vec2 v12 = vertices1[iv2];

	glm::vec2 localTangent = v12 - v11;
	localTangent = glm::normalize(localTangent);

	glm::vec2 localNormal = MathUtils::Cross2(localTangent, 1.0f);
	glm::vec2 planePoint = 0.5f * (v11 + v12);

	glm::vec2 tangent = Rotation2D::Mul(xf1.q, localTangent);
	glm::vec2 normal = MathUtils::Cross2(tangent, 1.0f);

	v11 = Transform2D::Mul(xf1, v11);
	v12 = Transform2D::Mul(xf1, v12);

	// Face offset.
	real32 frontOffset = glm::dot(normal, v11);

	// Side offsets, extended by polytope skin thickness.
	real32 sideOffset1 = -glm::dot(tangent, v11) + totalRadius;
	real32 sideOffset2 = glm::dot(tangent, v12) + totalRadius;

	// Clip incident edge against extruded edge1 side edges.
	ClipVertex clipPoints1[2];
	ClipVertex clipPoints2[2];
	int np;

	// Clip to box side 1
	np = ClipSegmentToLine(clipPoints1, incidentEdge, -tangent, sideOffset1, iv1);

	if (np < 2)
		return;

	// Clip to negative box side 1
	np = ClipSegmentToLine(clipPoints2, clipPoints1,  tangent, sideOffset2, iv2);

	if (np < 2)
	{
		return;
	}

	// Now clipPoints2 contains the clipped points.
	manifold->localNormal = localNormal;
	manifold->localPoint = planePoint;

	s32 pointCount = 0;
	for (s32 i = 0; i < maxManifoldPoints; ++i)
	{
		real32 separation = glm::dot(normal, clipPoints2[i].v) - frontOffset;

		if (separation <= totalRadius)
		{
			ManifoldPoint* cp = manifold->points + pointCount;
			cp->localPoint = Transform2D::MulT(xf2, clipPoints2[i].v);
			cp->id = clipPoints2[i].id;
			if (flip)
			{
				// Swap features
				ContactFeature cf = cp->id.cf;
				cp->id.cf.indexA = cf.indexB;
				cp->id.cf.indexB = cf.indexA;
				cp->id.cf.typeA = cf.typeB;
				cp->id.cf.typeB = cf.typeA;
			}
			++pointCount;
		}
	}

	manifold->pointCount = pointCount;
}
