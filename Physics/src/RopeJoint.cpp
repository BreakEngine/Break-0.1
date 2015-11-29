#include "RopeJoint.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;


// Limit:
// C = norm(pB - pA) - L
// u = (pB - pA) / norm(pB - pA)
// Cdot = dot(u, vB + cross(wB, rB) - vA - cross(wA, rA))
// J = [-u -cross(rA, u) u cross(rB, u)]
// K = J * invM * JT
//   = invMassA + invIA * cross(rA, u)^2 + invMassB + invIB * cross(rB, u)^2

RopeJoint::RopeJoint(const RopeJointDef* def) : Joint(def)
{
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;

	m_maxLength = def->maxLength;

	m_mass = 0.0f;
	m_impulse = 0.0f;
	m_state = inactiveLimit;
	m_length = 0.0f;
}

void RopeJoint::InitVelocityConstraints(const SolverData& data)
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
	m_u = cB + m_rB - cA - m_rA;

	m_length = m_u.length();

	real32 C = m_length - m_maxLength;
	if (C > 0.0f)
	{
		m_state = atUpperLimit;
	}
	else
	{
		m_state = inactiveLimit;
	}

	if (m_length > linearSlop)
	{
		m_u *= 1.0f / m_length;
	}
	else
	{
		m_u = glm::vec2(0,0);
		m_mass = 0.0f;
		m_impulse = 0.0f;
		return;
	}

	// Compute effective mass.
	real32 crA = MathUtils::Cross2(m_rA, m_u);
	real32 crB = MathUtils::Cross2(m_rB, m_u);
	real32 invMass = m_invMassA + m_invIA * crA * crA + m_invMassB + m_invIB * crB * crB;

	m_mass = invMass != 0.0f ? 1.0f / invMass : 0.0f;

	if (data.step.warmStarting)
	{
		// Scale the impulse to support a variable time step.
		m_impulse *= data.step.dtRatio;

		glm::vec2 P = m_impulse * m_u;
		vA -= m_invMassA * P;
		wA -= m_invIA * MathUtils::Cross2(m_rA, P);
		vB += m_invMassB * P;
		wB += m_invIB * MathUtils::Cross2(m_rB, P);
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

void RopeJoint::SolveVelocityConstraints(const SolverData& data)
{
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	// Cdot = dot(u, v + cross(w, r))
	glm::vec2 vpA = vA + MathUtils::Cross2(wA, m_rA);
	glm::vec2 vpB = vB + MathUtils::Cross2(wB, m_rB);
	real32 C = m_length - m_maxLength;
	real32 Cdot = glm::dot(m_u, vpB - vpA);

	// Predictive constraint.
	if (C < 0.0f)
	{
		Cdot += data.step.inv_dt * C;
	}

	real32 impulse = -m_mass * Cdot;
	real32 oldImpulse = m_impulse;
	m_impulse = glm::min(0.0f, m_impulse + impulse);
	impulse = m_impulse - oldImpulse;

	glm::vec2 P = impulse * m_u;
	vA -= m_invMassA * P;
	wA -= m_invIA * MathUtils::Cross2(m_rA, P);
	vB += m_invMassB * P;
	wB += m_invIB * MathUtils::Cross2(m_rB, P);

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

bool RopeJoint::SolvePositionConstraints(const SolverData& data)
{
	glm::vec2 cA = data.positions[m_indexA].c;
	real32 aA = data.positions[m_indexA].a;
	glm::vec2 cB = data.positions[m_indexB].c;
	real32 aB = data.positions[m_indexB].a;

	Rotation2D qA(aA), qB(aB);

	glm::vec2 rA = Rotation2D::Mul(qA, m_localAnchorA - m_localCenterA);
	glm::vec2 rB = Rotation2D::Mul(qB, m_localAnchorB - m_localCenterB);
	glm::vec2 u = cB + rB - cA - rA;

	real32 length = u.length();
	real32 C = length - m_maxLength;

	C = glm::clamp(C, 0.0f, maxLinearCorrection);

	real32 impulse = -m_mass * C;
	glm::vec2 P = impulse * u;

	cA -= m_invMassA * P;
	aA -= m_invIA * MathUtils::Cross2(rA, P);
	cB += m_invMassB * P;
	aB += m_invIB * MathUtils::Cross2(rB, P);

	data.positions[m_indexA].c = cA;
	data.positions[m_indexA].a = aA;
	data.positions[m_indexB].c = cB;
	data.positions[m_indexB].a = aB;

	return length - m_maxLength < linearSlop;
}

glm::vec2 RopeJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

glm::vec2 RopeJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

glm::vec2 RopeJoint::GetReactionForce(real32 inv_dt) const
{
	glm::vec2 F = (inv_dt * m_impulse) * m_u;
	return F;
}

real32 RopeJoint::GetReactionTorque(real32 inv_dt) const
{
	NOT_USED(inv_dt);
	return 0.0f;
}

real32 RopeJoint::GetMaxLength() const
{
	return m_maxLength;
}

LimitState RopeJoint::GetLimitState() const
{
	return m_state;
}

void RopeJoint::Dump()
{
	s32 indexA = m_bodyA->m_islandIndex;
	s32 indexB = m_bodyB->m_islandIndex;

	printf("  RopeJointDef jd;\n");
	printf("  jd.bodyA = bodies[%d];\n", indexA);
	printf("  jd.bodyB = bodies[%d];\n", indexB);
	printf("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	printf("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	printf("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	printf("  jd.maxLength = %.15lef;\n", m_maxLength);
	printf("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}
