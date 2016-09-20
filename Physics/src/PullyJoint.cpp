#include "PullyJoint.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;



// Pulley:
// length1 = norm(p1 - s1)
// length2 = norm(p2 - s2)
// C0 = (length1 + ratio * length2)_initial
// C = C0 - (length1 + ratio * length2)
// u1 = (p1 - s1) / norm(p1 - s1)
// u2 = (p2 - s2) / norm(p2 - s2)
// Cdot = -dot(u1, v1 + MathUtils::Cross2(w1, r1)) - ratio * dot(u2, v2 + MathUtils::Cross2(w2, r2))
// J = -[u1 MathUtils::Cross2(r1, u1) ratio * u2  ratio * MathUtils::Cross2(r2, u2)]
// K = J * invM * JT
//   = invMass1 + invI1 * MathUtils::Cross2(r1, u1)^2 + ratio^2 * (invMass2 + invI2 * MathUtils::Cross2(r2, u2)^2)

void PulleyJointDef::Initialize(Body* bA, Body* bB,
								const glm::vec2& groundA, const glm::vec2& groundB,
								const glm::vec2& anchorA, const glm::vec2& anchorB,
								real32 r)
{
	bodyA = bA;
	bodyB = bB;
	groundAnchorA = groundA;
	groundAnchorB = groundB;
	localAnchorA = bodyA->GetLocalPoint(anchorA);
	localAnchorB = bodyB->GetLocalPoint(anchorB);
	glm::vec2 dA = anchorA - groundA;
	lengthA = dA.length();
	glm::vec2 dB = anchorB - groundB;
	lengthB = dB.length();
	ratio = r;
	assert(ratio > FLT_EPSILON);
}

PulleyJoint::PulleyJoint(const PulleyJointDef* def)
	: Joint(def)
{
	m_groundAnchorA = def->groundAnchorA;
	m_groundAnchorB = def->groundAnchorB;
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;

	m_lengthA = def->lengthA;
	m_lengthB = def->lengthB;

	assert(def->ratio != 0.0f);
	m_ratio = def->ratio;

	m_constant = def->lengthA + m_ratio * def->lengthB;

	m_impulse = 0.0f;
}

void PulleyJoint::InitVelocityConstraints(const SolverData& data)
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
	
	m_rA = Rotation2D::Mul(qA, m_localAnchorA - m_localCenterA);
	m_rB = Rotation2D::Mul(qB, m_localAnchorB - m_localCenterB);

	// Get the pulley axes.
	m_uA = cA + m_rA - m_groundAnchorA;
	m_uB = cB + m_rB - m_groundAnchorB;

	real32 lengthA = m_uA.length();
	real32 lengthB = m_uB.length();

	if (lengthA > 10.0f * linearSlop)
	{
		m_uA *= 1.0f / lengthA;
	}
	else
	{
		m_uA = glm::vec2(0,0);
	}

	if (lengthB > 10.0f * linearSlop)
	{
		m_uB *= 1.0f / lengthB;
	}
	else
	{
		m_uB = glm::vec2(0,0);
	}

	// Compute effective mass.
	real32 ruA = MathUtils::Cross2(m_rA, m_uA);
	real32 ruB = MathUtils::Cross2(m_rB, m_uB);

	real32 mA = m_invMassA + m_invIA * ruA * ruA;
	real32 mB = m_invMassB + m_invIB * ruB * ruB;

	m_mass = mA + m_ratio * m_ratio * mB;

	if (m_mass > 0.0f)
	{
		m_mass = 1.0f / m_mass;
	}

	if (data.step.warmStarting)
	{
		// Scale impulses to support variable time steps.
		m_impulse *= data.step.dtRatio;

		// Warm starting.
		glm::vec2 PA = -(m_impulse) * m_uA;
		glm::vec2 PB = (-m_ratio * m_impulse) * m_uB;

		vA += m_invMassA * PA;
		wA += m_invIA * MathUtils::Cross2(m_rA, PA);
		vB += m_invMassB * PB;
		wB += m_invIB * MathUtils::Cross2(m_rB, PB);
	}
	else
	{
		m_impulse = 0.0f;
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

void PulleyJoint::SolveVelocityConstraints(const SolverData& data)
{
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	glm::vec2 vpA = vA + MathUtils::Cross2(wA, m_rA);
	glm::vec2 vpB = vB + MathUtils::Cross2(wB, m_rB);

	real32 Cdot = -glm::dot(m_uA, vpA) - m_ratio * glm::dot(m_uB, vpB);
	real32 impulse = -m_mass * Cdot;
	m_impulse += impulse;

	glm::vec2 PA = -impulse * m_uA;
	glm::vec2 PB = -m_ratio * impulse * m_uB;
	vA += m_invMassA * PA;
	wA += m_invIA * MathUtils::Cross2(m_rA, PA);
	vB += m_invMassB * PB;
	wB += m_invIB * MathUtils::Cross2(m_rB, PB);

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

bool PulleyJoint::SolvePositionConstraints(const SolverData& data)
{
	glm::vec2 cA = data.positions[m_indexA].c;
	real32 aA = data.positions[m_indexA].a;
	glm::vec2 cB = data.positions[m_indexB].c;
	real32 aB = data.positions[m_indexB].a;

	Rotation2D qA(aA), qB(aB);

	glm::vec2 rA = Rotation2D::Mul(qA, m_localAnchorA - m_localCenterA);
	glm::vec2 rB = Rotation2D::Mul(qB, m_localAnchorB - m_localCenterB);

	// Get the pulley axes.
	glm::vec2 uA = cA + rA - m_groundAnchorA;
	glm::vec2 uB = cB + rB - m_groundAnchorB;

	real32 lengthA = uA.length();
	real32 lengthB = uB.length();

	if (lengthA > 10.0f * linearSlop)
	{
		uA *= 1.0f / lengthA;
	}
	else
	{
		uA = glm::vec2(0,0);
	}

	if (lengthB > 10.0f * linearSlop)
	{
		uB *= 1.0f / lengthB;
	}
	else
	{
		uB = glm::vec2(0,0);
	}

	// Compute effective mass.
	real32 ruA = MathUtils::Cross2(rA, uA);
	real32 ruB = MathUtils::Cross2(rB, uB);

	real32 mA = m_invMassA + m_invIA * ruA * ruA;
	real32 mB = m_invMassB + m_invIB * ruB * ruB;

	real32 mass = mA + m_ratio * m_ratio * mB;

	if (mass > 0.0f)
	{
		mass = 1.0f / mass;
	}

	real32 C = m_constant - lengthA - m_ratio * lengthB;
	real32 linearError = glm::abs(C);

	real32 impulse = -mass * C;

	glm::vec2 PA = -impulse * uA;
	glm::vec2 PB = -m_ratio * impulse * uB;

	cA += m_invMassA * PA;
	aA += m_invIA * MathUtils::Cross2(rA, PA);
	cB += m_invMassB * PB;
	aB += m_invIB * MathUtils::Cross2(rB, PB);

	data.positions[m_indexA].c = cA;
	data.positions[m_indexA].a = aA;
	data.positions[m_indexB].c = cB;
	data.positions[m_indexB].a = aB;

	return linearError < linearSlop;
}

glm::vec2 PulleyJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

glm::vec2 PulleyJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

glm::vec2 PulleyJoint::GetReactionForce(real32 inv_dt) const
{
	glm::vec2 P = m_impulse * m_uB;
	return inv_dt * P;
}

real32 PulleyJoint::GetReactionTorque(real32 inv_dt) const
{
	NOT_USED(inv_dt);
	return 0.0f;
}

glm::vec2 PulleyJoint::GetGroundAnchorA() const
{
	return m_groundAnchorA;
}

glm::vec2 PulleyJoint::GetGroundAnchorB() const
{
	return m_groundAnchorB;
}

real32 PulleyJoint::GetLengthA() const
{
	return m_lengthA;
}

real32 PulleyJoint::GetLengthB() const
{
	return m_lengthB;
}

real32 PulleyJoint::GetRatio() const
{
	return m_ratio;
}

real32 PulleyJoint::GetCurrentLengthA() const
{
	glm::vec2 p = m_bodyA->GetWorldPoint(m_localAnchorA);
	glm::vec2 s = m_groundAnchorA;
	glm::vec2 d = p - s;
	return d.length();
}

real32 PulleyJoint::GetCurrentLengthB() const
{
	glm::vec2 p = m_bodyB->GetWorldPoint(m_localAnchorB);
	glm::vec2 s = m_groundAnchorB;
	glm::vec2 d = p - s;
	return d.length();
}

void PulleyJoint::Dump()
{
	s32 indexA = m_bodyA->m_islandIndex;
	s32 indexB = m_bodyB->m_islandIndex;

	printf("  PulleyJointDef jd;\n");
	printf("  jd.bodyA = bodies[%d];\n", indexA);
	printf("  jd.bodyB = bodies[%d];\n", indexB);
	printf("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	printf("  jd.groundAnchorA.Set(%.15lef, %.15lef);\n", m_groundAnchorA.x, m_groundAnchorA.y);
	printf("  jd.groundAnchorB.Set(%.15lef, %.15lef);\n", m_groundAnchorB.x, m_groundAnchorB.y);
	printf("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	printf("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	printf("  jd.lengthA = %.15lef;\n", m_lengthA);
	printf("  jd.lengthB = %.15lef;\n", m_lengthB);
	printf("  jd.ratio = %.15lef;\n", m_ratio);
	printf("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}

void PulleyJoint::ShiftOrigin(const glm::vec2& newOrigin)
{
	m_groundAnchorA -= newOrigin;
	m_groundAnchorB -= newOrigin;
}