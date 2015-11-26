#include "Collision.hpp"
#include "Distance.hpp"
#include "TimeOfImpact.hpp"
#include "CircleShape.hpp"
#include "PolygonShape.hpp"
#include "TimeManager.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


real32 toiTime, toiMaxTime;
s32 toiCalls, toiIters, toiMaxIters;
s32 toiRootIters, toiMaxRootIters;

//
struct BREAK_API SeparationFunction
{
	enum Type
	{
		points,
		faceA,
		faceB
	};

	real32 Initialize(const SimplexCache* cache,const DistanceProxy* proxyA, const Sweep& sweepA,const DistanceProxy* proxyB, const Sweep& sweepB,real32 t1)
	{
		m_proxyA = proxyA;
		m_proxyB = proxyB;
		s32 _count = cache->count;

		if(_count == 3) 
		printf("bugged count : %d \n",_count);

		assert(0 < _count && _count < 3);

		m_sweepA = sweepA;
		m_sweepB = sweepB;

		Transform2D xfA, xfB;
		m_sweepA.GetTransform(&xfA, t1);
		m_sweepB.GetTransform(&xfB, t1);

		if (_count == 1)
		{
			m_type = points;
			glm::vec2 localPointA = m_proxyA->GetVertex(cache->indexA[0]);
			glm::vec2 localPointB = m_proxyB->GetVertex(cache->indexB[0]);
			glm::vec2 pointA = MathUtils::Mul(xfA, localPointA);
			glm::vec2 pointB = MathUtils::Mul(xfB, localPointB);
			m_axis = pointB - pointA;
			real32 s = m_axis.length();
			return s;
		}
		else if (cache->indexA[0] == cache->indexA[1])
		{
			// Two points on B and one on A.
			m_type = faceB;
			glm::vec2 localPointB1 = proxyB->GetVertex(cache->indexB[0]);
			glm::vec2 localPoint = proxyB->GetVertex(cache->indexB[1]);

			m_axis = MathUtils::Cross2(localPoint - localPointB1, 1.0f);
			m_axis = glm::normalize(m_axis);
			glm::vec2 normal = MathUtils::Mul(xfB.q, m_axis);

			m_localPoint = 0.5f * (localPointB1 + localPoint);
			glm::vec2 pointB = MathUtils::Mul(xfB, m_localPoint);

			glm::vec2 localPointA = proxyA->GetVertex(cache->indexA[0]);
			glm::vec2 pointA = MathUtils::Mul(xfA, localPointA);

			real32 s = glm::dot(pointA - pointB, normal);
			if (s < 0.0f)
			{
				m_axis = -m_axis;
				s = -s;
			}
			return s;
		}
		else
		{
			// Two points on A and one or two points on B.
			m_type = faceA;
			glm::vec2 localPointA1 = m_proxyA->GetVertex(cache->indexA[0]);
			glm::vec2 localPointA2 = m_proxyA->GetVertex(cache->indexA[1]);

			m_axis = MathUtils::Cross2(localPointA2 - localPointA1, 1.0f);
			m_axis = glm::normalize(m_axis);
			glm::vec2 normal = MathUtils::Mul(xfA.q, m_axis);

			m_localPoint = 0.5f * (localPointA1 + localPointA2);
			glm::vec2 pointA = MathUtils::Mul(xfA, m_localPoint);

			glm::vec2 localPointB = m_proxyB->GetVertex(cache->indexB[0]);
			glm::vec2 pointB = MathUtils::Mul(xfB, localPointB);

			real32 s = glm::dot(pointB - pointA, normal);
			if (s < 0.0f)
			{
				m_axis = -m_axis;
				s = -s;
			}
			return s;
		}
	}

	//
	real32 FindMinSeparation(s32* indexA, s32* indexB, real32 t) const
	{
		Transform2D xfA, xfB;
		m_sweepA.GetTransform(&xfA, t);
		m_sweepB.GetTransform(&xfB, t);

		switch (m_type)
		{
		case points:
			{
				glm::vec2 axisA = MathUtils::MulT(xfA.q,  m_axis);
				glm::vec2 axisB = MathUtils::MulT(xfB.q, -m_axis);

				*indexA = m_proxyA->GetSupport(axisA);
				*indexB = m_proxyB->GetSupport(axisB);

				glm::vec2 localPointA = m_proxyA->GetVertex(*indexA);
				glm::vec2 localPointB = m_proxyB->GetVertex(*indexB);

				glm::vec2 pointA = MathUtils::Mul(xfA, localPointA);
				glm::vec2 pointB = MathUtils::Mul(xfB, localPointB);

				real32 separation = glm::dot(pointB - pointA, m_axis);
				return separation;
			}

		case faceA:
			{
				glm::vec2 normal = MathUtils::Mul(xfA.q, m_axis);
				glm::vec2 pointA = MathUtils::Mul(xfA, m_localPoint);

				glm::vec2 axisB = MathUtils::MulT(xfB.q, -normal);

				*indexA = -1;
				*indexB = m_proxyB->GetSupport(axisB);

				glm::vec2 localPointB = m_proxyB->GetVertex(*indexB);
				glm::vec2 pointB = MathUtils::Mul(xfB, localPointB);

				real32 separation = glm::dot(pointB - pointA, normal);
				return separation;
			}

		case faceB:
			{
				glm::vec2 normal = MathUtils::Mul(xfB.q, m_axis);
				glm::vec2 pointB = MathUtils::Mul(xfB, m_localPoint);

				glm::vec2 axisA = MathUtils::MulT(xfA.q, -normal);

				*indexB = -1;
				*indexA = m_proxyA->GetSupport(axisA);

				glm::vec2 localPointA = m_proxyA->GetVertex(*indexA);
				glm::vec2 pointA = MathUtils::Mul(xfA, localPointA);

				real32 separation = glm::dot(pointA - pointB, normal);
				return separation;
			}

		default:
			assert(false);
			*indexA = -1;
			*indexB = -1;
			return 0.0f;
		}
	}

	//
	real32 Evaluate(s32 indexA, s32 indexB, real32 t) const
	{
		Transform2D xfA, xfB;
		m_sweepA.GetTransform(&xfA, t);
		m_sweepB.GetTransform(&xfB, t);

		switch (m_type)
		{
		case points:
			{
				glm::vec2 localPointA = m_proxyA->GetVertex(indexA);
				glm::vec2 localPointB = m_proxyB->GetVertex(indexB);

				glm::vec2 pointA = MathUtils::Mul(xfA, localPointA);
				glm::vec2 pointB = MathUtils::Mul(xfB, localPointB);
				real32 separation = glm::dot(pointB - pointA, m_axis);

				return separation;
			}

		case faceA:
			{
				glm::vec2 normal = MathUtils::Mul(xfA.q, m_axis);
				glm::vec2 pointA = MathUtils::Mul(xfA, m_localPoint);

				glm::vec2 localPointB = m_proxyB->GetVertex(indexB);
				glm::vec2 pointB = MathUtils::Mul(xfB, localPointB);

				real32 separation = glm::dot(pointB - pointA, normal);
				return separation;
			}

		case faceB:
			{
				glm::vec2 normal = MathUtils::Mul(xfB.q, m_axis);
				glm::vec2 pointB = MathUtils::Mul(xfB, m_localPoint);

				glm::vec2 localPointA = m_proxyA->GetVertex(indexA);
				glm::vec2 pointA = MathUtils::Mul(xfA, localPointA);

				real32 separation = glm::dot(pointA - pointB, normal);
				return separation;
			}

		default:
			assert(false);
			return 0.0f;
		}
	}

	const DistanceProxy* m_proxyA;
	const DistanceProxy* m_proxyB;
	Sweep m_sweepA, m_sweepB;
	Type m_type;
	glm::vec2 m_localPoint;
	glm::vec2 m_axis;
};

// CCD via the local separating axis method. This seeks progression
// by computing the largest time at which separation is maintained.
void physics::TimeOfImpact(TOIOutput* output, const TOIInput* input)
{
	Time timer;

	++toiCalls;

	output->state = TOIOutput::unknown;
	output->t = input->tMax;

	const DistanceProxy* proxyA = &input->proxyA;
	const DistanceProxy* proxyB = &input->proxyB;

	Sweep sweepA = input->sweepA;
	Sweep sweepB = input->sweepB;

	// Large rotations can make the root finder fail, so we normalize the
	// sweep angles.
	sweepA.Normalize(); 
	sweepB.Normalize();

	real32 tMax = input->tMax;

	real32 totalRadius = proxyA->m_radius + proxyB->m_radius;
	real32 target = glm::max(linearSlop, totalRadius - 3.0f * linearSlop);
	real32 tolerance = 0.25f * linearSlop;
	assert(target > tolerance);

	real32 t1 = 0.0f;
	const s32 k_maxIterations = 20;	// TODO_ERIN Settings
	s32 iter = 0;

	// Prepare input for distance query.
	SimplexCache cache;
	cache.count = 0;
	DistanceInput distanceInput;
	distanceInput.proxyA = input->proxyA;
	distanceInput.proxyB = input->proxyB;
	distanceInput.useRadii = false;

	// The outer loop progressively attempts to compute new separating axes.
	// This loop terminates when an axis is repeated (no progress is made).
	for(;;)
	{
		Transform2D xfA, xfB;
		sweepA.GetTransform(&xfA, t1);
		sweepB.GetTransform(&xfB, t1);

		// Get the distance between shapes. We can also use the results
		// to get a separating axis.
		distanceInput.Transform2DA = xfA;
		distanceInput.Transform2DB = xfB;
		DistanceOutput distanceOutput;
		Distance(&distanceOutput, &cache, &distanceInput);

		// If the shapes are overlapped, we give up on continuous collision.
		if (distanceOutput.distance <= 0.0f)
		{
			// Failure!
			output->state = TOIOutput::overlapped;
			output->t = 0.0f;
			break;
		}

		if (distanceOutput.distance < target + tolerance)
		{
			// Victory!
			output->state = TOIOutput::touching;
			output->t = t1;
			break;
		}

		// Initialize the separating axis.
		SeparationFunction fcn;
		fcn.Initialize(&cache, proxyA, sweepA, proxyB, sweepB, t1);
#if 0
		// Dump the curve seen by the root finder
		{
			const s32 N = 100;
			real32 dx = 1.0f / N;
			real32 xs[N+1];
			real32 fs[N+1];

			real32 x = 0.0f;

			for (s32 i = 0; i <= N; ++i)
			{
				sweepA.GetTransform2D(&xfA, x);
				sweepB.GetTransform2D(&xfB, x);
				real32 f = fcn.Evaluate(xfA, xfB) - target;

				printf("%g %g\n", x, f);

				xs[i] = x;
				fs[i] = f;

				x += dx;
			}
		}
#endif

		// Compute the TOI on the separating axis. We do this by successively
		// resolving the deepest point. This loop is bounded by the number of vertices.
		bool done = false;
		real32 t2 = tMax;
		s32 pushBackIter = 0;
		for (;;)
		{
			// Find the deepest point at t2. Store the witness point indices.
			s32 indexA, indexB;
			real32 s2 = fcn.FindMinSeparation(&indexA, &indexB, t2);

			// Is the final configuration separated?
			if (s2 > target + tolerance)
			{
				// Victory!
				output->state = TOIOutput::separated;
				output->t = tMax;
				done = true;
				break;
			}

			// Has the separation reached tolerance?
			if (s2 > target - tolerance)
			{
				// Advance the sweeps
				t1 = t2;
				break;
			}

			// Compute the initial separation of the witness points.
			real32 s1 = fcn.Evaluate(indexA, indexB, t1);

			// Check for initial overlap. This might happen if the root finder
			// runs out of iterations.
			if (s1 < target - tolerance)
			{
				output->state = TOIOutput::failed;
				output->t = t1;
				done = true;
				break;
			}

			// Check for touching
			if (s1 <= target + tolerance)
			{
				// Victory! t1 should hold the TOI (could be 0.0).
				output->state = TOIOutput::touching;
				output->t = t1;
				done = true;
				break;
			}

			// Compute 1D root of: f(x) - target = 0
			s32 rootIterCount = 0;
			real32 a1 = t1, a2 = t2;
			for (;;)
			{
				// Use a mix of the secant rule and bisection.
				real32 t;
				if (rootIterCount & 1)
				{
					// Secant rule to improve convergence.
					t = a1 + (target - s1) * (a2 - a1) / (s2 - s1);
				}
				else
				{
					// Bisection to guarantee progress.
					t = 0.5f * (a1 + a2);
				}

				++rootIterCount;
				++toiRootIters;

				real32 s = fcn.Evaluate(indexA, indexB, t);

				if (glm::abs(s - target) < tolerance)
				{
					// t2 holds a tentative value for t1
					t2 = t;
					break;
				}

				// Ensure we continue to bracket the root.
				if (s > target)
				{
					a1 = t;
					s1 = s;
				}
				else
				{
					a2 = t;
					s2 = s;
				}

				if (rootIterCount == 50)
				{
					break;
				}
			}

			toiMaxRootIters = glm::max(toiMaxRootIters, rootIterCount);

			++pushBackIter;

			if (pushBackIter == maxPolygonVertices)
			{
				break;
			}
		}

		++iter;
		++toiIters;

		if (done)
		{
			break;
		}

		if (iter == k_maxIterations)
		{
			// Root finder got stuck. Semi-victory.
			output->state = TOIOutput::failed;
			output->t = t1;
			break;
		}
	}

	toiMaxIters = glm::max(toiMaxIters, iter);

	real32 time = timer.GetMilliseconds();
	toiMaxTime = glm::max(toiMaxTime, time);
	toiTime += time;
}






