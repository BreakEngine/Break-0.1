#include "ContactSolver.hpp"
#include "Contact2D.hpp"
#include "Body2D.hpp"
#include "Fixture.hpp"
#include "World2D.hpp"
#include "StackAllocator.hpp"


using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;


#define _DEBUG_SOLVER 0

struct Physics::ContactPositionConstraint
{
	glm::vec2 localPoints[maxManifoldPoints];
	glm::vec2 localNormal;
	glm::vec2 localPoint;
	s32 indexA;
	s32 indexB;
	real32 invMassA, invMassB;
	glm::vec2 localCenterA, localCenterB;
	real32 invIA, invIB;
	Manifold::Type type;
	real32 radiusA, radiusB;
	s32 pointCount;
};

ContactSolver::ContactSolver(ContactSolverDef* def)
{
	m_step = def->step;
	m_allocator = def->allocator;
	m_count = def->count;
	m_positionConstraints = (ContactPositionConstraint*)m_allocator->Allocate(m_count * sizeof(ContactPositionConstraint));
	m_velocityConstraints = (ContactVelocityConstraint*)m_allocator->Allocate(m_count * sizeof(ContactVelocityConstraint));
	m_positions = def->positions;
	m_velocities = def->velocities;
	m_contacts = def->contacts;

	// Initialize position independent portions of the constraints.
	for (s32 i = 0; i < m_count; ++i)
	{
		Contact* contact = m_contacts[i];

		Fixture* fixtureA = contact->m_fixtureA;
		Fixture* fixtureB = contact->m_fixtureB;
		Shape* shapeA = fixtureA->GetShape();
		Shape* shapeB = fixtureB->GetShape();
		real32 radiusA = shapeA->m_radius;
		real32 radiusB = shapeB->m_radius;
		Body* bodyA = fixtureA->GetBody();
		Body* bodyB = fixtureB->GetBody();
		Manifold* manifold = contact->GetManifold();

		s32 pointCount = manifold->pointCount;
		assert(pointCount > 0);

		ContactVelocityConstraint* vc = m_velocityConstraints + i;
		vc->friction = contact->m_friction;
		vc->restitution = contact->m_restitution;
		vc->tangentSpeed = contact->m_tangentSpeed;
		vc->indexA = bodyA->m_islandIndex;
		vc->indexB = bodyB->m_islandIndex;
		vc->invMassA = bodyA->m_invMass;
		vc->invMassB = bodyB->m_invMass;
		vc->invIA = bodyA->m_invI;
		vc->invIB = bodyB->m_invI;
		vc->contactIndex = i;
		vc->pointCount = pointCount;
		vc->K = glm::mat2(0.0f,0.0f,0.0f,0.0f);
		vc->normalMass = glm::mat2(0.0f,0.0f,0.0f,0.0f);

		ContactPositionConstraint* pc = m_positionConstraints + i;
		pc->indexA = bodyA->m_islandIndex;
		pc->indexB = bodyB->m_islandIndex;
		pc->invMassA = bodyA->m_invMass;
		pc->invMassB = bodyB->m_invMass;
		pc->localCenterA = bodyA->m_sweep.localCenter;
		pc->localCenterB = bodyB->m_sweep.localCenter;
		pc->invIA = bodyA->m_invI;
		pc->invIB = bodyB->m_invI;
		pc->localNormal = manifold->localNormal;
		pc->localPoint = manifold->localPoint;
		pc->pointCount = pointCount;
		pc->radiusA = radiusA;
		pc->radiusB = radiusB;
		pc->type = manifold->type;

		for (s32 j = 0; j < pointCount; ++j)
		{
			ManifoldPoint* cp = manifold->points + j;
			VelocityConstraintPoint* vcp = vc->points + j;

			if (m_step.warmStarting)
			{
				vcp->normalImpulse = m_step.dtRatio * cp->normalImpulse;
				vcp->tangentImpulse = m_step.dtRatio * cp->tangentImpulse;
			}
			else
			{
				vcp->normalImpulse = 0.0f;
				vcp->tangentImpulse = 0.0f;
			}

			vcp->rA = glm::vec2(0.0f,0.0f);
			vcp->rB = glm::vec2(0.0f,0.0f);
			vcp->normalMass = 0.0f;
			vcp->tangentMass = 0.0f;
			vcp->velocityBias = 0.0f;

			pc->localPoints[j] = cp->localPoint;
		}
	}
}

ContactSolver::~ContactSolver()
{
	m_allocator->Free(m_velocityConstraints);
	m_allocator->Free(m_positionConstraints);
}

// Initialize position dependent portions of the velocity constraints.
void ContactSolver::InitializeVelocityConstraints()
{
	for (s32 i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints + i;
		ContactPositionConstraint* pc = m_positionConstraints + i;

		real32 radiusA = pc->radiusA;
		real32 radiusB = pc->radiusB;
		Manifold* manifold = m_contacts[vc->contactIndex]->GetManifold();

		s32 indexA = vc->indexA;
		s32 indexB = vc->indexB;

		real32 mA = vc->invMassA;
		real32 mB = vc->invMassB;
		real32 iA = vc->invIA;
		real32 iB = vc->invIB;
		glm::vec2 localCenterA = pc->localCenterA;
		glm::vec2 localCenterB = pc->localCenterB;

		glm::vec2 cA = m_positions[indexA].c;
		real32 aA = m_positions[indexA].a;
		glm::vec2 vA = m_velocities[indexA].v;
		real32 wA = m_velocities[indexA].w;

		glm::vec2 cB = m_positions[indexB].c;
		real32 aB = m_positions[indexB].a;
		glm::vec2 vB = m_velocities[indexB].v;
		real32 wB = m_velocities[indexB].w;

		assert(manifold->pointCount > 0);

		Transform2D xfA, xfB;
		xfA.q.Set(aA);
		xfB.q.Set(aB);
		xfA.p = cA - Rotation2D::Mul(xfA.q, localCenterA);
		xfB.p = cB - Rotation2D::Mul(xfB.q, localCenterB);

		WorldManifold worldManifold;
		worldManifold.Initialize(manifold, xfA, radiusA, xfB, radiusB);

		vc->normal = worldManifold.normal;

		s32 pointCount = vc->pointCount;
		for (s32 j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;

			vcp->rA = worldManifold.points[j] - cA;
			vcp->rB = worldManifold.points[j] - cB;

			real32 rnA = MathUtils::Cross2(vcp->rA, vc->normal);
			real32 rnB = MathUtils::Cross2(vcp->rB, vc->normal);

			real32 kNormal = mA + mB + iA * rnA * rnA + iB * rnB * rnB;

			vcp->normalMass = kNormal > 0.0f ? 1.0f / kNormal : 0.0f;

			glm::vec2 tangent = MathUtils::Cross2(vc->normal, 1.0f);

			real32 rtA = MathUtils::Cross2(vcp->rA, tangent);
			real32 rtB = MathUtils::Cross2(vcp->rB, tangent);

			real32 kTangent = mA + mB + iA * rtA * rtA + iB * rtB * rtB;

			vcp->tangentMass = kTangent > 0.0f ? 1.0f /  kTangent : 0.0f;

			// Setup a velocity bias for restitution.
			vcp->velocityBias = 0.0f;
			real32 vRel = glm::dot(vc->normal, vB + MathUtils::Cross2(wB, vcp->rB) - vA - MathUtils::Cross2(wA, vcp->rA));
			if (vRel < -velocityThreshold)
			{
				vcp->velocityBias = -vc->restitution * vRel;
			}
		}

		// If we have two points, then prepare the block solver.
		if (vc->pointCount == 2)
		{
			VelocityConstraintPoint* vcp1 = vc->points + 0;
			VelocityConstraintPoint* vcp2 = vc->points + 1;

			real32 rn1A = MathUtils::Cross2(vcp1->rA, vc->normal);
			real32 rn1B = MathUtils::Cross2(vcp1->rB, vc->normal);
			real32 rn2A = MathUtils::Cross2(vcp2->rA, vc->normal);
			real32 rn2B = MathUtils::Cross2(vcp2->rB, vc->normal);

			real32 k11 = mA + mB + iA * rn1A * rn1A + iB * rn1B * rn1B;
			real32 k22 = mA + mB + iA * rn2A * rn2A + iB * rn2B * rn2B;
			real32 k12 = mA + mB + iA * rn1A * rn2A + iB * rn1B * rn2B;

			// Ensure a reasonable condition number.
			const real32 k_maxConditionNumber = 1000.0f;
			if (k11 * k11 < k_maxConditionNumber * (k11 * k22 - k12 * k12))
			{
				// K is safe to invert.
				//vc->K.ex.Set(k11, k12);
				//vc->K.ey.Set(k12, k22);
				vc->K = glm::mat2(k11,k12,k12,k22);

				//vc->normalMass = vc->K.GetInverse();
				vc->normalMass = glm::inverse(vc->K);
			}
			else
			{
				// The constraints are redundant, just use one.
				vc->pointCount = 1;
			}
		}
	}
}

void ContactSolver::WarmStart()
{
	// Warm start.
	for (s32 i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints + i;

		s32 indexA = vc->indexA;
		s32 indexB = vc->indexB;
		real32 mA = vc->invMassA;
		real32 iA = vc->invIA;
		real32 mB = vc->invMassB;
		real32 iB = vc->invIB;
		s32 pointCount = vc->pointCount;

		glm::vec2 vA = m_velocities[indexA].v;
		real32 wA = m_velocities[indexA].w;
		glm::vec2 vB = m_velocities[indexB].v;
		real32 wB = m_velocities[indexB].w;

		glm::vec2 normal = vc->normal;
		glm::vec2 tangent = MathUtils::Cross2(normal, 1.0f);

		for (s32 j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;
			glm::vec2 P = vcp->normalImpulse * normal + vcp->tangentImpulse * tangent;
			wA -= iA * MathUtils::Cross2(vcp->rA, P);
			vA -= mA * P;
			wB += iB * MathUtils::Cross2(vcp->rB, P);
			vB += mB * P;
		}

		m_velocities[indexA].v = vA;
		m_velocities[indexA].w = wA;
		m_velocities[indexB].v = vB;
		m_velocities[indexB].w = wB;
	}
}

void ContactSolver::SolveVelocityConstraints()
{
	for (s32 i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints + i;

		s32 indexA = vc->indexA;
		s32 indexB = vc->indexB;
		real32 mA = vc->invMassA;
		real32 iA = vc->invIA;
		real32 mB = vc->invMassB;
		real32 iB = vc->invIB;
		s32 pointCount = vc->pointCount;

		glm::vec2 vA = m_velocities[indexA].v;
		real32 wA = m_velocities[indexA].w;
		glm::vec2 vB = m_velocities[indexB].v;
		real32 wB = m_velocities[indexB].w;

		glm::vec2 normal = vc->normal;
		glm::vec2 tangent = MathUtils::Cross2(normal, 1.0f);
		real32 friction = vc->friction;

		assert(pointCount == 1 || pointCount == 2);

		// Solve tangent constraints first because non-penetration is more important
		// than friction.
		for (s32 j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;

			// Relative velocity at contact
			glm::vec2 dv = vB + MathUtils::Cross2(wB, vcp->rB) - vA - MathUtils::Cross2(wA, vcp->rA);

			// Compute tangent force
			real32 vt = glm::dot(dv, tangent) - vc->tangentSpeed;
			real32 lambda = vcp->tangentMass * (-vt);

			// Clamp the accumulated force
			real32 maxFriction = friction * vcp->normalImpulse;
			real32 newImpulse = glm::clamp(vcp->tangentImpulse + lambda, -maxFriction, maxFriction);
			lambda = newImpulse - vcp->tangentImpulse;
			vcp->tangentImpulse = newImpulse;

			// Apply contact impulse
			glm::vec2 P = lambda * tangent;

			vA -= mA * P;
			wA -= iA * MathUtils::Cross2(vcp->rA, P);

			vB += mB * P;
			wB += iB * MathUtils::Cross2(vcp->rB, P);
		}

		// Solve normal constraints
		if (vc->pointCount == 1)
		{
			VelocityConstraintPoint* vcp = vc->points + 0;

			// Relative velocity at contact
			glm::vec2 dv = vB + MathUtils::Cross2(wB, vcp->rB) - vA - MathUtils::Cross2(wA, vcp->rA);

			// Compute normal impulse
			real32 vn = glm::dot(dv, normal);
			real32 lambda = -vcp->normalMass * (vn - vcp->velocityBias);

			// Clamp the accumulated impulse
			real32 newImpulse = glm::max(vcp->normalImpulse + lambda, 0.0f);
			lambda = newImpulse - vcp->normalImpulse;
			vcp->normalImpulse = newImpulse;

			// Apply contact impulse
			glm::vec2 P = lambda * normal;
			vA -= mA * P;
			wA -= iA * MathUtils::Cross2(vcp->rA, P);

			vB += mB * P;
			wB += iB * MathUtils::Cross2(vcp->rB, P);
		}
		else
		{
			// Block solver developed in collaboration with Dirk Gregorius (back in 01/07 on Box2D_Lite).
			// Build the mini LCP for this contact patch
			//
			// vn = A * x + b, vn >= 0, , vn >= 0, x >= 0 and vn_i * x_i = 0 with i = 1..2
			//
			// A = J * W * JT and J = ( -n, -r1 x n, n, r2 x n )
			// b = vn0 - velocityBias
			//
			// The system is solved using the "Total enumeration method" (s. Murty). The complementary constraint vn_i * x_i
			// implies that we must have in any solution either vn_i = 0 or x_i = 0. So for the 2D contact problem the cases
			// vn1 = 0 and vn2 = 0, x1 = 0 and x2 = 0, x1 = 0 and vn2 = 0, x2 = 0 and vn1 = 0 need to be tested. The first valid
			// solution that satisfies the problem is chosen.
			// 
			// In order to account of the accumulated impulse 'a' (because of the iterative nature of the solver which only requires
			// that the accumulated impulse is clamped and not the incremental impulse) we change the impulse variable (x_i).
			//
			// Substitute:
			// 
			// x = a + d
			// 
			// a := old total impulse
			// x := new total impulse
			// d := incremental impulse 
			//
			// For the current iteration we extend the formula for the incremental impulse
			// to compute the new total impulse:
			//
			// vn = A * d + b
			//    = A * (x - a) + b
			//    = A * x + b - A * a
			//    = A * x + b'
			// b' = b - A * a;

			VelocityConstraintPoint* cp1 = vc->points + 0;
			VelocityConstraintPoint* cp2 = vc->points + 1;

			glm::vec2 a(cp1->normalImpulse, cp2->normalImpulse);
			assert(a.x >= 0.0f && a.y >= 0.0f);

			// Relative velocity at contact
			glm::vec2 dv1 = vB + MathUtils::Cross2(wB, cp1->rB) - vA - MathUtils::Cross2(wA, cp1->rA);
			glm::vec2 dv2 = vB + MathUtils::Cross2(wB, cp2->rB) - vA - MathUtils::Cross2(wA, cp2->rA);

			// Compute normal velocity
			real32 vn1 = glm::dot(dv1, normal);
			real32 vn2 = glm::dot(dv2, normal);

			glm::vec2 b;
			b.x = vn1 - cp1->velocityBias;
			b.y = vn2 - cp2->velocityBias;

			// Compute b'
			//b -= Mul(vc->K, a);
			b -= vc->K * a;
			

			const real32 k_errorTol = 1e-3f;
			NOT_USED(k_errorTol);

			for (;;)
			{
				//
				// Case 1: vn = 0
				//
				// 0 = A * x + b'
				//
				// Solve for x:
				//
				// x = - inv(A) * b'
				//
				//glm::vec2 x = - Mul(vc->normalMass, b);
				glm::vec2 x = - (vc->normalMass * b);

				if (x.x >= 0.0f && x.y >= 0.0f)
				{
					// Get the incremental impulse
					glm::vec2 d = x - a;

					// Apply incremental impulse
					glm::vec2 P1 = d.x * normal;
					glm::vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (MathUtils::Cross2(cp1->rA, P1) + MathUtils::Cross2(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (MathUtils::Cross2(cp1->rB, P1) + MathUtils::Cross2(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

#if _DEBUG_SOLVER == 1
					// Postconditions
					dv1 = vB + MathUtils::Cross2(wB, cp1->rB) - vA - MathUtils::Cross2(wA, cp1->rA);
					dv2 = vB + MathUtils::Cross2(wB, cp2->rB) - vA - MathUtils::Cross2(wA, cp2->rA);

					// Compute normal velocity
					vn1 = Dot(dv1, normal);
					vn2 = Dot(dv2, normal);

					assert(Abs(vn1 - cp1->velocityBias) < k_errorTol);
					assert(Abs(vn2 - cp2->velocityBias) < k_errorTol);
#endif
					break;
				}

				//
				// Case 2: vn1 = 0 and x2 = 0
				//
				//   0 = a11 * x1 + a12 * 0 + b1' 
				// vn2 = a21 * x1 + a22 * 0 + '
				//
				x.x = - cp1->normalMass * b.x;
				x.y = 0.0f;
				vn1 = 0.0f;
				//vn2 = vc->K.ex.y * x.x + b.y;
				vn2 = vc->K[0][1] * x.x + b.y;   //<<===== to test here ((shaalan))

				if (x.x >= 0.0f && vn2 >= 0.0f)
				{
					// Get the incremental impulse
					glm::vec2 d = x - a;

					// Apply incremental impulse
					glm::vec2 P1 = d.x * normal;
					glm::vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (MathUtils::Cross2(cp1->rA, P1) + MathUtils::Cross2(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (MathUtils::Cross2(cp1->rB, P1) + MathUtils::Cross2(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

#if _DEBUG_SOLVER == 1
					// Postconditions
					dv1 = vB + MathUtils::Cross2(wB, cp1->rB) - vA - MathUtils::Cross2(wA, cp1->rA);

					// Compute normal velocity
					vn1 = Dot(dv1, normal);

					assert(Abs(vn1 - cp1->velocityBias) < k_errorTol);
#endif
					break;
				}


				//
				// Case 3: vn2 = 0 and x1 = 0
				//
				// vn1 = a11 * 0 + a12 * x2 + b1' 
				//   0 = a21 * 0 + a22 * x2 + '
				//
				x.x = 0.0f;
				x.y = - cp2->normalMass * b.y;
				//vn1 = vc->K.ey.x * x.y + b.x;
				vn1 = vc->K[1][0] * x.y + b.x;  //<<======= to test here ((shaalan))
				vn2 = 0.0f;

				if (x.y >= 0.0f && vn1 >= 0.0f)
				{
					// Resubstitute for the incremental impulse
					glm::vec2 d = x - a;

					// Apply incremental impulse
					glm::vec2 P1 = d.x * normal;
					glm::vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (MathUtils::Cross2(cp1->rA, P1) + MathUtils::Cross2(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (MathUtils::Cross2(cp1->rB, P1) + MathUtils::Cross2(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

#if _DEBUG_SOLVER == 1
					// Postconditions
					dv2 = vB + MathUtils::Cross2(wB, cp2->rB) - vA - MathUtils::Cross2(wA, cp2->rA);

					// Compute normal velocity
					vn2 = Dot(dv2, normal);

					assert(Abs(vn2 - cp2->velocityBias) < k_errorTol);
#endif
					break;
				}

				//
				// Case 4: x1 = 0 and x2 = 0
				// 
				// vn1 = b1
				// vn2 = b2;
				x.x = 0.0f;
				x.y = 0.0f;
				vn1 = b.x;
				vn2 = b.y;

				if (vn1 >= 0.0f && vn2 >= 0.0f )
				{
					// Resubstitute for the incremental impulse
					glm::vec2 d = x - a;

					// Apply incremental impulse
					glm::vec2 P1 = d.x * normal;
					glm::vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (MathUtils::Cross2(cp1->rA, P1) + MathUtils::Cross2(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (MathUtils::Cross2(cp1->rB, P1) + MathUtils::Cross2(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

					break;
				}

				// No solution, give up. This is hit sometimes, but it doesn't seem to matter.
				break;
			}
		}

		m_velocities[indexA].v = vA;
		m_velocities[indexA].w = wA;
		m_velocities[indexB].v = vB;
		m_velocities[indexB].w = wB;
	}
}

void ContactSolver::StoreImpulses()
{
	for (s32 i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints + i;
		Manifold* manifold = m_contacts[vc->contactIndex]->GetManifold();

		for (s32 j = 0; j < vc->pointCount; ++j)
		{
			manifold->points[j].normalImpulse = vc->points[j].normalImpulse;
			manifold->points[j].tangentImpulse = vc->points[j].tangentImpulse;
		}
	}
}

struct PositionSolverManifold
{
	void Initialize(ContactPositionConstraint* pc, const Transform2D& xfA, const Transform2D& xfB, s32 index)
	{
		assert(pc->pointCount > 0);

		switch (pc->type)
		{
		case Manifold::circles:
			{
				glm::vec2 pointA = Transform2D::Mul(xfA, pc->localPoint);
				glm::vec2 pointB = Transform2D::Mul(xfB, pc->localPoints[0]);
				normal = pointB - pointA;
				normal = glm::normalize(normal);
				point = 0.5f * (pointA + pointB);
				separation = glm::dot(pointB - pointA, normal) - pc->radiusA - pc->radiusB;
			}
			break;

		case Manifold::faceA:
			{
				normal = Rotation2D::Mul(xfA.q, pc->localNormal);
				glm::vec2 planePoint = Transform2D::Mul(xfA, pc->localPoint);

				glm::vec2 clipPoint = Transform2D::Mul(xfB, pc->localPoints[index]);
				separation = glm::dot(clipPoint - planePoint, normal) - pc->radiusA - pc->radiusB;
				point = clipPoint;
			}
			break;

		case Manifold::faceB:
			{
				normal = Rotation2D::Mul(xfB.q, pc->localNormal);
				glm::vec2 planePoint = Transform2D::Mul(xfB, pc->localPoint);

				glm::vec2 clipPoint = Transform2D::Mul(xfA, pc->localPoints[index]);
				separation = glm::dot(clipPoint - planePoint, normal) - pc->radiusA - pc->radiusB;
				point = clipPoint;

				// Ensure normal points from A to B
				normal = -normal;
			}
			break;
		}
	}

	glm::vec2 normal;
	glm::vec2 point;
	real32 separation;
};

// Sequential solver.
bool ContactSolver::SolvePositionConstraints()
{
	real32 minSeparation = 0.0f;

	for (s32 i = 0; i < m_count; ++i)
	{
		ContactPositionConstraint* pc = m_positionConstraints + i;

		s32 indexA = pc->indexA;
		s32 indexB = pc->indexB;
		glm::vec2 localCenterA = pc->localCenterA;
		real32 mA = pc->invMassA;
		real32 iA = pc->invIA;
		glm::vec2 localCenterB = pc->localCenterB;
		real32 mB = pc->invMassB;
		real32 iB = pc->invIB;
		s32 pointCount = pc->pointCount;

		glm::vec2 cA = m_positions[indexA].c;
		real32 aA = m_positions[indexA].a;

		glm::vec2 cB = m_positions[indexB].c;
		real32 aB = m_positions[indexB].a;

		// Solve normal constraints
		for (s32 j = 0; j < pointCount; ++j)
		{
			Transform2D xfA, xfB;
			xfA.q.Set(aA);
			xfB.q.Set(aB);
			xfA.p = cA - Rotation2D::Mul(xfA.q, localCenterA);
			xfB.p = cB - Rotation2D::Mul(xfB.q, localCenterB);

			PositionSolverManifold psm;
			psm.Initialize(pc, xfA, xfB, j);
			glm::vec2 normal = psm.normal;

			glm::vec2 point = psm.point;
			real32 separation = psm.separation;

			glm::vec2 rA = point - cA;
			glm::vec2 rB = point - cB;

			// Track max constraint error.
			minSeparation = glm::min(minSeparation, separation);

			// Prevent large corrections and allow slop.
			real32 C = glm::clamp(baumgarte * (separation + linearSlop), - maxLinearCorrection, 0.0f);


			// Compute the effective mass.
			real32 rnA = MathUtils::Cross2(rA, normal);
			real32 rnB = MathUtils::Cross2(rB, normal);
			real32 K = mA + mB + iA * rnA * rnA + iB * rnB * rnB;

			// Compute normal impulse
			real32 impulse = K > 0.0f ? - C / K : 0.0f;

			glm::vec2 P = impulse * normal;

			cA -= mA * P;
			aA -= iA * MathUtils::Cross2(rA, P);

			cB += mB * P;
			aB += iB * MathUtils::Cross2(rB, P);
		}

		m_positions[indexA].c = cA;
		m_positions[indexA].a = aA;

		m_positions[indexB].c = cB;
		m_positions[indexB].a = aB;
	}

	// We can't expect minSpeparation >= -_linearSlop because we don't
	// push the separation above -_linearSlop.
	return minSeparation >= -3.0f * linearSlop;
}

// Sequential position solver for position constraints.
bool ContactSolver::SolveTOIPositionConstraints(s32 toiIndexA, s32 toiIndexB)
{
	real32 minSeparation = 0.0f;

	for (s32 i = 0; i < m_count; ++i)
	{
		ContactPositionConstraint* pc = m_positionConstraints + i;

		s32 indexA = pc->indexA;
		s32 indexB = pc->indexB;
		glm::vec2 localCenterA = pc->localCenterA;
		glm::vec2 localCenterB = pc->localCenterB;
		s32 pointCount = pc->pointCount;

		real32 mA = 0.0f;
		real32 iA = 0.0f;
		if (indexA == toiIndexA || indexA == toiIndexB)
		{
			mA = pc->invMassA;
			iA = pc->invIA;
		}

		real32 mB = 0.0f;
		real32 iB = 0.;
		if (indexB == toiIndexA || indexB == toiIndexB)
		{
			mB = pc->invMassB;
			iB = pc->invIB;
		}

		glm::vec2 cA = m_positions[indexA].c;
		real32 aA = m_positions[indexA].a;

		glm::vec2 cB = m_positions[indexB].c;
		real32 aB = m_positions[indexB].a;

		// Solve normal constraints
		for (s32 j = 0; j < pointCount; ++j)
		{
			Transform2D xfA, xfB;
			xfA.q.Set(aA);
			xfB.q.Set(aB);
			xfA.p = cA - Rotation2D::Mul(xfA.q, localCenterA);
			xfB.p = cB - Rotation2D::Mul(xfB.q, localCenterB);

			PositionSolverManifold psm;
			psm.Initialize(pc, xfA, xfB, j);
			glm::vec2 normal = psm.normal;

			glm::vec2 point = psm.point;
			real32 separation = psm.separation;

			glm::vec2 rA = point - cA;
			glm::vec2 rB = point - cB;

			// Track max constraint error.
			minSeparation = glm::min(minSeparation, separation);

			// Prevent large corrections and allow slop.
			real32 C = glm::clamp(toiBaugarte * (separation + linearSlop), - maxLinearCorrection, 0.0f);

			// Compute the effective mass.
			real32 rnA = MathUtils::Cross2(rA, normal);
			real32 rnB = MathUtils::Cross2(rB, normal);
			real32 K = mA + mB + iA * rnA * rnA + iB * rnB * rnB;

			// Compute normal impulse
			real32 impulse = K > 0.0f ? - C / K : 0.0f;

			glm::vec2 P = impulse * normal;

			cA -= mA * P;
			aA -= iA * MathUtils::Cross2(rA, P);

			cB += mB * P;
			aB += iB * MathUtils::Cross2(rB, P);
		}

		m_positions[indexA].c = cA;
		m_positions[indexA].a = aA;

		m_positions[indexB].c = cB;
		m_positions[indexB].a = aB;
	}

	// We can't expect minSpeparation >= -_linearSlop because we don't
	// push the separation above -_linearSlop.
	return minSeparation >= -1.5f * linearSlop;
}
