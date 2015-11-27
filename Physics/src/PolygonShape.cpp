#include "PolygonShape.hpp"
#include <new>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;



//get new allocation for shape with spicific type..
Shape* PolygonShape::Clone(BlockAllocator* allocator) const
{
	void* mem = allocator->Allocate(sizeof(PolygonShape));
	PolygonShape* clone = new (mem) PolygonShape;
	*clone = *this;
	return clone;
}



void PolygonShape::SetAsBox(real32 hx, real32 hy)
{
	m_count = 4;
	m_vertices[0] = glm::vec2(-hx, -hy);
	m_vertices[1] = glm::vec2( hx, -hy);
	m_vertices[2] = glm::vec2( hx,  hy);
	m_vertices[3] = glm::vec2(-hx,  hy);

	m_normals[0] = glm::vec2(0.0f, -1.0f);
	m_normals[1] = glm::vec2(1.0f, 0.0f);
	m_normals[2] = glm::vec2(0.0f, 1.0f);
	m_normals[3] = glm::vec2(-1.0f, 0.0f);

	m_centroid.x = 0; 
	m_centroid.y = 0; 
}

void PolygonShape::SetAsBox(real32 hx, real32 hy, const glm::vec2& center, real32 angle)
{
	m_count = 4;
	m_vertices[0] = glm::vec2(-hx, -hy);
	m_vertices[1] = glm::vec2( hx, -hy);
	m_vertices[2] = glm::vec2( hx,  hy);
	m_vertices[3] = glm::vec2(-hx,  hy);
	m_normals[0] = glm::vec2(0.0f, -1.0f);
	m_normals[1] = glm::vec2(1.0f, 0.0f);
	m_normals[2] = glm::vec2(0.0f, 1.0f);
	m_normals[3] = glm::vec2(-1.0f, 0.0f);
	m_centroid = center;

	Transform2D xf;
	xf.p = center;
	xf.q.Set(angle);

	// Transform2D vertices and normals.
	for (s32 i = 0; i < m_count; ++i)
	{
		m_vertices[i] = MathUtils::Mul(xf, m_vertices[i]);
		m_normals[i] =  MathUtils::Mul(xf.q, m_normals[i]);
	}
}


//return just one child because its one shape ...
s32 PolygonShape::GetChildCount() const
{
	return 1;
}


//compute origin of any polygon..
static glm::vec2 ComputeCentroid(const glm::vec2* vs, s32 count)
{
	assert(count >= 3);

	glm::vec2 c = glm::vec2(0.0f, 0.0f);
	real32 area = 0.0f;

	// pRef is the reference point for forming triangles.
	// It's location doesn't change the result (except for rounding error).
	glm::vec2 pRef = glm::vec2(0.0f, 0.0f);
#if 0
	// This code would put the reference point inside the polygon.
	for (s32 i = 0; i < count; ++i)
	{
		pRef += vs[i];
	}
	pRef *= 1.0f / count;
#endif

	const real32 inv3 = 1.0f / 3.0f;

	for (s32 i = 0; i < count; ++i)
	{
		// Triangle vertices.
		glm::vec2 p1 = pRef;
		glm::vec2 p2 = vs[i];
		glm::vec2 p3 = i + 1 < count ? vs[i+1] : vs[0];

		glm::vec2 e1 = p2 - p1;
		glm::vec2 e2 = p3 - p1;

		real32 D = MathUtils::Cross2(e1, e2);

		real32 triangleArea = 0.5f * D;
		area += triangleArea;

		// Area weighted centroid
		c += triangleArea * inv3 * (p1 + p2 + p3);
	}

	// Centroid
	assert(area > FLT_EPSILON);
	c *= 1.0f / area;
	return c;
}

void PolygonShape::Set(const glm::vec2* vertices, s32 count)
{
	assert(3 <= count && count <= maxPolygonVertices);
	if (count < 3)
	{
		SetAsBox(1.0f, 1.0f);
		return;
	}

	s32 n = glm::min(count, maxPolygonVertices);

	// Perform welding and copy vertices into local buffer.
	glm::vec2 ps[maxPolygonVertices];
	s32 tempCount = 0;
	for (s32 i = 0; i < n; ++i)
	{
		glm::vec2 v = vertices[i];

		bool unique = true;
		for (s32 j = 0; j < tempCount; ++j)
		{
			if (MathUtils::DistanceSquared(v, ps[j]) < 0.5f * linearSlop)   //using of linear slop
			{
				unique = false;
				break;
			}
		}

		if (unique)
		{
			ps[tempCount++] = v;
		}
	}

	n = tempCount;
	if (n < 3)
	{
		// Polygon is degenerate.
		assert(false);
		SetAsBox(1.0f, 1.0f);
		return;
	}

	// Create the convex hull using the ((Gift wrapping algorithm)) <<-----
	// http://en.wikipedia.org/wiki/Gift_wrapping_algorithm

	// Find the right most point on the hull
	s32 i0 = 0;
	real32 x0 = ps[0].x;
	for (s32 i = 1; i < n; ++i)
	{
		real32 x = ps[i].x;
		if (x > x0 || (x == x0 && ps[i].y < ps[i0].y))
		{
			i0 = i;
			x0 = x;
		}
	}

	s32 hull[maxPolygonVertices];
	s32 m = 0;
	s32 ih = i0;

	for (;;)
	{
		hull[m] = ih;

		s32 ie = 0;
		for (s32 j = 1; j < n; ++j)
		{
			if (ie == ih)
			{
				ie = j;
				continue;
			}

			glm::vec2 r = ps[ie] - ps[hull[m]];
			glm::vec2 v = ps[j] - ps[hull[m]];
			real32 c = MathUtils::Cross2(r, v);
			if (c < 0.0f)
			{
				ie = j;
			}

			// Collinearity check
			
			if (c == 0.0f && MathUtils::LengthSquared(v) > MathUtils::LengthSquared(r))
			{
				ie = j;
			}
		}

		++m;
		ih = ie;

		if (ie == i0)
		{
			break;
		}
	}

	m_count = m;

	// Copy vertices.
	for (s32 i = 0; i < m; ++i)
	{
		m_vertices[i] = ps[hull[i]];
	}

	// Compute normals. Ensure the edges have non-zero length.
	for (s32 i = 0; i < m; ++i)
	{
		s32 i1 = i;
		s32 i2 = i + 1 < m ? i + 1 : 0;
		glm::vec2 edge = m_vertices[i2] - m_vertices[i1];
		assert( MathUtils::LengthSquared(edge) > FLT_EPSILON * FLT_EPSILON);
		m_normals[i] = MathUtils::Cross2(edge, 1.0f);

		m_normals[i] = glm::normalize(m_normals[i]);
	}

	// Compute the polygon centroid.
	m_centroid = ComputeCentroid(m_vertices, m);
}

bool PolygonShape::TestPoint(const Transform2D& xf, const glm::vec2& p) const
{
	glm::vec2 pLocal = MathUtils::MulT(xf.q, p - xf.p);

	for (s32 i = 0; i < m_count; ++i)
	{
		real32 dot = glm::dot(m_normals[i], pLocal - m_vertices[i]);
		if (dot > 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool PolygonShape::RayCast(RayCastOutput* output, const RayCastInput& input,const Transform2D& xf, s32 childIndex) const
{
	NOT_USED(childIndex);  //using of not_used <<-----

	// Put the ray into the polygon's frame of reference.
	glm::vec2 p1 = MathUtils::MulT(xf.q, input.p1 - xf.p);
	glm::vec2 p2 = MathUtils::MulT(xf.q, input.p2 - xf.p);
	glm::vec2 d = p2 - p1;

	real32 lower = 0.0f, upper = input.maxFraction;

	s32 index = -1;

	for (s32 i = 0; i < m_count; ++i)
	{
		// p = p1 + a * d
		// dot(normal, p - v) = 0
		// dot(normal, p1 - v) + a * dot(normal, d) = 0
		real32 numerator = glm::dot(m_normals[i], m_vertices[i] - p1);
		real32 denominator = glm::dot(m_normals[i], d);

		if (denominator == 0.0f)
		{	
			if (numerator < 0.0f)
			{
				return false;
			}
		}
		else
		{
			// Note: we want this predicate without division:
			// lower < numerator / denominator, where denominator < 0
			// Since denominator < 0, we have to flip the inequality:
			// lower < numerator / denominator <==> denominator * lower > numerator.
			if (denominator < 0.0f && numerator < lower * denominator)
			{
				// Increase lower.
				// The segment enters this half-space.
				lower = numerator / denominator;
				index = i;
			}
			else if (denominator > 0.0f && numerator < upper * denominator)
			{
				// Decrease upper.
				// The segment exits this half-space.
				upper = numerator / denominator;
			}
		}

		// The use of _epsilon here causes the assert on lower to trip
		// in some cases. Apparently the use of _epsilon was to make edge
		// shapes work, but now those are handled separately.
		//if (upper < lower - __epsilon)
		if (upper < lower)
		{
			return false;
		}
	}

	assert(0.0f <= lower && lower <= input.maxFraction);

	if (index >= 0)
	{
		output->fraction = lower;
		output->normal = MathUtils::Mul(xf.q, m_normals[index]);
		return true;
	}

	return false;
}

void PolygonShape::ComputeAABB(AABB* aabb, const Transform2D& xf, s32 childIndex) const
{
	NOT_USED(childIndex);

	glm::vec2 lower = MathUtils::Mul(xf, m_vertices[0]);
	glm::vec2 upper = lower;

	for (s32 i = 1; i < m_count; ++i)
	{
		glm::vec2 v = MathUtils::Mul(xf, m_vertices[i]);
		lower = glm::min(lower, v);
		upper = glm::max(upper, v);
	}

	glm::vec2 r(m_radius, m_radius);
	aabb->lowerBound = lower - r;
	aabb->upperBound = upper + r;
}

void PolygonShape::ComputeMass(MassData* massData, real32 density) const
{
	// Polygon mass, centroid, and inertia.
	// Let rho be the polygon density in mass per unit area.
	// Then:
	// mass = rho * int(dA)
	// centroid.x = (1/mass) * rho * int(x * dA)
	// centroid.y = (1/mass) * rho * int(y * dA)
	// I = rho * int((x*x + y*y) * dA)
	//
	// We can compute these integrals by summing all the integrals
	// for each triangle of the polygon. To evaluate the integral
	// for a single triangle, we make a change of variables to
	// the (u,v) coordinates of the triangle:
	// x = x0 + e1x * u + e2x * v
	// y = y0 + e1y * u + e2y * v
	// where 0 <= u && 0 <= v && u + v <= 1.
	//
	// We integrate u from [0,1-v] and then v from [0,1].
	// We also need to use the Jacobian of the Transform2Dation:
	// D = cross(e1, e2)
	//
	// Simplification: triangle centroid = (1/3) * (p1 + p2 + p3)
	//
	// The rest of the derivation is handled by computer algebra.

	assert(m_count >= 3);

	glm::vec2 center; center = glm::vec2(0.0f, 0.0f);
	real32 area = 0.0f;
	real32 I = 0.0f;

	// s is the reference point for forming triangles.
	// It's location doesn't change the result (except for rounding error).
	glm::vec2 s(0.0f, 0.0f);

	// This code would put the reference point inside the polygon.
	for (s32 i = 0; i < m_count; ++i)
	{
		s += m_vertices[i];
	}
	s *= 1.0f / m_count;

	const real32 k_inv3 = 1.0f / 3.0f;

	for (s32 i = 0; i < m_count; ++i)
	{
		// Triangle vertices.
		glm::vec2 e1 = m_vertices[i] - s;
		glm::vec2 e2 = i + 1 < m_count ? m_vertices[i+1] - s : m_vertices[0] - s;

		real32 D = MathUtils::Cross2(e1, e2);

		real32 triangleArea = 0.5f * D;
		area += triangleArea;

		// Area weighted centroid
		center += triangleArea * k_inv3 * (e1 + e2);

		real32 ex1 = e1.x, ey1 = e1.y;
		real32 ex2 = e2.x, ey2 = e2.y;

		real32 intx2 = ex1*ex1 + ex2*ex1 + ex2*ex2;
		real32 inty2 = ey1*ey1 + ey2*ey1 + ey2*ey2;

		I += (0.25f * k_inv3 * D) * (intx2 + inty2);
	}

	// Total mass
	massData->mass = density * area;

	// Center of mass
	assert(area > FLT_EPSILON);
	center *= 1.0f / area;
	massData->center = center + s;

	// Inertia tensor relative to the local origin (point s).
	massData->I = density * I;

	// Shift to center of mass then to original body origin.
	massData->I += massData->mass * (glm::dot(massData->center, massData->center) - glm::dot(center, center));
}

bool PolygonShape::Validate() const
{
	for (s32 i = 0; i < m_count; ++i)
	{
		s32 i1 = i;
		s32 i2 = i < m_count - 1 ? i1 + 1 : 0;
		glm::vec2 p = m_vertices[i1];
		glm::vec2 e = m_vertices[i2] - p;

		for (s32 j = 0; j < m_count; ++j)
		{
			if (j == i1 || j == i2)
			{
				continue;
			}

			glm::vec2 v = m_vertices[j] - p;
			real32 c = MathUtils::Cross2(e, v);
			if (c < 0.0f)
			{
				return false;
			}
		}
	}

	return true;
}
