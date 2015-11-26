#include "MotorJoint.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


// Point-to-point constraint
// Cdot = v2 - v1
//      = v2 + MathUtils::Cross2(w2, r2) - v1 - MathUtils::Cross2(w1, r1)
// J = [-I -r1_skew I r2_skew ]
// Identity used:
// w k % (rx i + ry j) = w * (-ry i + rx j)

// Angle constraint
// Cdot = w2 - w1
// J = [0 0 -1 0 0 1]
// K = invI1 + invI2

void MotorJointDef::Initialize(Body* bA, Body* bB)
{
	bodyA = bA;
	bodyB = bB;
	glm::vec2 xB = bodyB->GetPosition();
	linearOffset = bodyA->GetLocalPoint(xB);

	real32 angleA = bodyA->GetAngle();
	real32 angleB = bodyB->GetAngle();
	angularOffset = angleB - angleA;
}

MotorJoint::MotorJoint(const MotorJointDef* def)
	: Joint(def)
{
	m_linearOffset = def->linearOffset;
	m_angularOffset = def->angularOffset;

	m_linearImpulse = glm::vec2(0,0);
	m_angularImpulse = 0.0f;

	m_maxForce = def->maxForce;
	m_maxTorque = def->maxTorque;
	m_correctionFactor = def->correctionFactor;
}

void MotorJoint::InitVelocityConstraints(const SolverData& data)
{
	m_indexA = m_bodyA->m_islandIndex;
	m_indexB = m_bodyB->m_islandIndex;
	m_localCenterA = m_bodyA->m_sweep.localCenter;
	m_localCenterB = m_bodyB->m_sweep.localCenter;
	m_invMassA = m_bodyA->m_invMass;
	m_invMassB = m_bodyB->m_invMass;
	m_invIA = m_bodyA->m_invI;
	m_invIB = m_bodyB->m_invI;

	glm::vec2 cA = data.positions[m_indexA].c;
	real32 aA = data.positions[m_indexA].a;
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;

	glm::vec2 cB = data.positions[m_indexB].c;
	real32 aB = data.positions[m_indexB].a;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	Rotation2D qA(aA), qB(aB);

	// Compute the effective mass matrix.
	m_rA = MathUtils::Mul(qA, -m_localCenterA);
	m_rB = MathUtils::Mul(qB, -m_localCenterB);

	// J = [-I -r1_skew I r2_skew]
	//     [ 0       -1 0       1]
	// r_skew = [-ry; rx]

	// Matlab
	// K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x,          -r1y*iA-r2y*iB]
	//     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB,           r1x*iA+r2x*iB]
	//     [          -r1y*iA-r2y*iB,           r1x*iA+r2x*iB,                   iA+iB]

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	glm::mat2 K;
	//K.ex.x = mA + mB + iA * m_rA.y * m_rA.y + iB * m_rB.y * m_rB.y;
	//K.ex.y = -iA * m_rA.x * m_rA.y - iB * m_rB.x * m_rB.y;
	//K.ey.x = K.ex.y;
	//K.ey.y = mA + mB + iA * m_rA.x * m_rA.x + iB * m_rB.x * m_rB.x;

	//test here ((shaalan))
	K[0][0] = mA + mB + iA * m_rA.y * m_rA.y + iB * m_rB.y * m_rB.y;
	K[0][1] = -iA * m_rA.x * m_rA.y - iB * m_rB.x * m_rB.y;
	K[1][0] = K[0][1];
	K[1][1] = mA + mB + iA * m_rA.x * m_rA.x + iB * m_rB.x * m_rB.x;

	m_linearMass = glm::inverse( K );

	m_angularMass = iA + iB;
	if (m_angularMass > 0.0f)
	{
		m_angularMass = 1.0f / m_angularMass;
	}

	m_linearError = cB + m_rB - cA - m_rA - MathUtils::Mul(qA, m_linearOffset);
	m_angularError = aB - aA - m_angularOffset;

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

void MotorJoint::SolveVelocityConstraints(const SolverData& data)
{
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	real32 h = data.step.delta;
	real32 inv_h = data.step.inv_dt;

	// Solve angular friction
	{
		real32 Cdot = wB - wA + inv_h * m_correctionFactor * m_angularError;
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
		glm::vec2 Cdot = vB + MathUtils::Cross2(wB, m_rB) - vA - MathUtils::Cross2(wA, m_rA) + inv_h * m_correctionFactor * m_linearError;

		glm::vec2 impulse = -(m_linearMass * Cdot);
		glm::vec2 oldImpulse = m_linearImpulse;
		m_linearImpulse += impulse;

		real32 maxImpulse = h * m_maxForce;

		if ( MathUtils::LengthSquared( m_linearImpulse) > maxImpulse * maxImpulse)
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

bool MotorJoint::SolvePositionConstraints(const SolverData& data)
{
	NOT_USED(data);

	return true;
}

glm::vec2 MotorJoint::GetAnchorA() const
{
	return m_bodyA->GetPosition();
}

glm::vec2 MotorJoint::GetAnchorB() const
{
	return m_bodyB->GetPosition();
}

glm::vec2 MotorJoint::GetReactionForce(real32 inv_dt) const
{
	return inv_dt * m_linearImpulse;
}

real32 MotorJoint::GetReactionTorque(real32 inv_dt) const
{
	return inv_dt * m_angularImpulse;
}

void MotorJoint::SetMaxForce(real32 force)
{
	assert(MathUtils::IsVal(force) && force >= 0.0f);
	m_maxForce = force;
}

real32 MotorJoint::GetMaxForce() const
{
	return m_maxForce;
}

void MotorJoint::SetMaxTorque(real32 torque)
{
	assert(MathUtils::IsVal(torque) && torque >= 0.0f);
	m_maxTorque = torque;
}

real32 MotorJoint::GetMaxTorque() const
{
	return m_maxTorque;
}

void MotorJoint::SetCorrectionFactor(real32 factor)
{
	assert(MathUtils::IsVal(factor) && 0.0f <= factor && factor <= 1.0f);
	m_correctionFactor = factor;
}

real32 MotorJoint::GetCorrectionFactor() const
{
	return m_correctionFactor;
}

void MotorJoint::SetLinearOffset(const glm::vec2& linearOffset)
{
	if (linearOffset.x != m_linearOffset.x || linearOffset.y != m_linearOffset.y)
	{
		m_bodyA->SetAwake(true);
		m_bodyB->SetAwake(true);
		m_linearOffset = linearOffset;
	}
}

const glm::vec2& MotorJoint::GetLinearOffset() const
{
	return m_linearOffset;
}

void MotorJoint::SetAngularOffset(real32 angularOffset)
{
	if (angularOffset != m_angularOffset)
	{
		m_bodyA->SetAwake(true);
		m_bodyB->SetAwake(true);
		m_angularOffset = angularOffset;
	}
}

real32 MotorJoint::GetAngularOffset() const
{
	return m_angularOffset;
}

void MotorJoint::Dump()
{
	s32 indexA = m_bodyA->m_islandIndex;
	s32 indexB = m_bodyB->m_islandIndex;

	printf("  MotorJointDef jd;\n");
	printf("  jd.bodyA = bodies[%d];\n", indexA);
	printf("  jd.bodyB = bodies[%d];\n", indexB);
	printf("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	printf("  jd.linearOffset.Set(%.15lef, %.15lef);\n", m_linearOffset.x, m_linearOffset.y);
	printf("  jd.angularOffset = %.15lef;\n", m_angularOffset);
	printf("  jd.maxForce = %.15lef;\n", m_maxForce);
	printf("  jd.maxTorque = %.15lef;\n", m_maxTorque);
	printf("  jd.correctionFactor = %.15lef;\n", m_correctionFactor);
	printf("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}

