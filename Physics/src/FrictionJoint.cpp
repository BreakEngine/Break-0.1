#include "FrictionJoint.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


// Point-to-point constraint
// Cdot = v2 - v1
//      = v2 + cross(w2, r2) - v1 - cross(w1, r1)
// J = [-I -r1_skew I r2_skew ]
// Identity used:
// w k % (rx i + ry j) = w * (-ry i + rx j)

// Angle constraint
// Cdot = w2 - w1
// J = [0 0 -1 0 0 1]
// K = invI1 + invI2

void FrictionJointDef::Initialize(Body* bA, Body* bB, const glm::vec2& anchor)
{
	bodyA = bA;
	bodyB = bB;
	localAnchorA = bodyA->GetLocalPoint(anchor);
	localAnchorB = bodyB->GetLocalPoint(anchor);
}

FrictionJoint::FrictionJoint(const FrictionJointDef* def)
	: Joint(def)
{
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;

	m_linearImpulse = glm::vec2(0,0);
	m_angularImpulse = 0.0f;

	m_maxForce = def->maxForce;
	m_maxTorque = def->maxTorque;
}

void FrictionJoint::InitVelocityConstraints(const SolverData& data)
{
	m_indexA = m_bodyA->m_islandIndex;
	m_indexB = m_bodyB->m_islandIndex;
	m_localCenterA = m_bodyA->m_sweep.localCenter;
	m_localCenterB = m_bodyB->m_sweep.localCenter;
	m_invMassA = m_bodyA->m_invMass;
	m_invMassB = m_bodyB->m_invMass;
	m_invIA = m_bodyA->m_invI;
	m_invIB = m_bodyB->m_invI;

	real32 aA = data.positions[m_indexA].a;
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;

	real32 aB = data.positions[m_indexB].a;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	Rotation2D qA(aA), qB(aB);

	// Compute the effective mass matrix.
	m_rA = MathUtils::Mul(qA, m_localAnchorA - m_localCenterA);
	m_rB = MathUtils::Mul(qB, m_localAnchorB - m_localCenterB);

	// J = [-I -r1_skew I r2_skew]
	//     [ 0       -1 0       1]
	// r_skew = [-ry; rx]

	// Matlab
	// K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x,          -r1y*iA-r2y*iB]
	//     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB,           r1x*iA+r2x*iB]
	//     [          -r1y*iA-r2y*iB,           r1x*iA+r2x*iB,                   iA+iB]

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;


	//<<===================to test here ((shaalan))
	glm::mat2 K;
	K[0][0] = mA + mB + iA * m_rA.y * m_rA.y + iB * m_rB.y * m_rB.y;
	K[1][0] = -iA * m_rA.x * m_rA.y - iB * m_rB.x * m_rB.y;
	K[0][1] = K[1][0];
	K[1][1] = mA + mB + iA * m_rA.x * m_rA.x + iB * m_rB.x * m_rB.x;

	m_linearMass = glm::inverse( K );

	m_angularMass = iA + iB;
	if (m_angularMass > 0.0f)
	{
		m_angularMass = 1.0f / m_angularMass;
	}

	if (data.step.warmStarting)
	{
		// Scale impulses to support a variable time step.
		m_linearImpulse *= data.step.dtRatio;
		m_angularImpulse *= data.step.dtRatio;

		glm::vec2 P(m_linearImpulse.x, m_linearImpulse.y);
		vA -= mA * P;
		wA -= iA * (MathUtils::Cross2(m_rA, P) + m_angularImpulse);
		vB += mB * P;
		wB += iB * (MathUtils::Cross2(m_rB, P) + m_angularImpulse);
	}
	else
	{
		m_linearImpulse = glm::vec2(0,0);
		m_angularImpulse = 0.0f;
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

void FrictionJoint::SolveVelocityConstraints(const SolverData& data)
{
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	real32 h = data.step.delta;

	// Solve angular friction
	{
		real32 Cdot = wB - wA;
		real32 impulse = -m_angularMass * Cdot;

		real32 oldImpulse = m_angularImpulse;
		real32 maxImpulse = h * m_maxTorque;
		m_angularImpulse = glm::clamp(m_angularImpulse + impulse, -maxImpulse, maxImpulse);
		impulse = m_angularImpulse - oldImpulse;

		wA -= iA * impulse;
		wB += iB * impulse;
	}

	// Solve linear friction
	{
		glm::vec2 Cdot = vB + MathUtils::Cross2(wB, m_rB) - vA - MathUtils::Cross2(wA, m_rA);

		//glm::vec2 impulse = -Mul(m_linearMass, Cdot);  //<<==== to test here ((shaalan))
		glm::vec2 impulse = - (m_linearMass * Cdot);

		glm::vec2 oldImpulse = m_linearImpulse;
		m_linearImpulse += impulse;

		real32 maxImpulse = h * m_maxForce;

		if ( MathUtils::LengthSquared( m_linearImpulse ) > maxImpulse * maxImpulse)
		{
			m_linearImpulse = glm::normalize(m_linearImpulse);
			m_linearImpulse *= maxImpulse;
		}

		impulse = m_linearImpulse - oldImpulse;

		vA -= mA * impulse;
		wA -= iA * MathUtils::Cross2(m_rA, impulse);

		vB += mB * impulse;
		wB += iB * MathUtils::Cross2(m_rB, impulse);
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

bool FrictionJoint::SolvePositionConstraints(const SolverData& data)
{
	NOT_USED(data);

	return true;
}

glm::vec2 FrictionJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

glm::vec2 FrictionJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

glm::vec2 FrictionJoint::GetReactionForce(real32 inv_dt) const
{
	return inv_dt * m_linearImpulse;
}

real32 FrictionJoint::GetReactionTorque(real32 inv_dt) const
{
	return inv_dt * m_angularImpulse;
}

void FrictionJoint::SetMaxForce(real32 force)
{
	assert(MathUtils::IsVal(force) && force >= 0.0f);
	m_maxForce = force;
}

real32 FrictionJoint::GetMaxForce() const
{
	return m_maxForce;
}

void FrictionJoint::SetMaxTorque(real32 torque)
{
	assert(MathUtils::IsVal(torque) && torque >= 0.0f);
	m_maxTorque = torque;
}

real32 FrictionJoint::GetMaxTorque() const
{
	return m_maxTorque;
}

void FrictionJoint::Dump()
{
	s32 indexA = m_bodyA->m_islandIndex;
	s32 indexB = m_bodyB->m_islandIndex;

	printf("  FrictionJointDef jd;\n");
	printf("  jd.bodyA = bodies[%d];\n", indexA);
	printf("  jd.bodyB = bodies[%d];\n", indexB);
	printf("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	printf("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	printf("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	printf("  jd.maxForce = %.15lef;\n", m_maxForce);
	printf("  jd.maxTorque = %.15lef;\n", m_maxTorque);
	printf("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}
