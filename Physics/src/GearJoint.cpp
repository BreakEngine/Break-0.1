#include "GearJoint.hpp"
#include "RevoluteJoint.hpp"
#include "PrismaticJoint.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"


using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


// Gear Joint:
// C0 = (coordinate1 + ratio * coordinate2)_initial
// C = (coordinate1 + ratio * coordinate2) - C0 = 0
// J = [J1 ratio * J2]
// K = J * invM * JT
//   = J1 * invM1 * J1T + ratio * ratio * J2 * invM2 * J2T
//
// Revolute:
// coordinate = rotation
// Cdot = angularVelocity
// J = [0 0 1]
// K = J * invM * JT = invI
//
// Prismatic:
// coordinate = dot(p - pg, ug)
// Cdot = dot(v + cross(w, r), ug)
// J = [ug cross(r, ug)]
// K = J * invM * JT = invMass + invI * cross(r, ug)^2

GearJoint::GearJoint(const GearJointDef* def)
	: Joint(def)
{
	m_joint1 = def->joint1;
	m_joint2 = def->joint2;

	m_typeA = m_joint1->GetType();
	m_typeB = m_joint2->GetType();

	assert(m_typeA == revoluteJoint || m_typeA == prismaticJoint);
	assert(m_typeB == revoluteJoint || m_typeB == prismaticJoint);

	real32 coordinateA, coordinateB;

	// TODO_ERIN there might be some problem with the joint edges in Joint.

	m_bodyC = m_joint1->GetBodyA();
	m_bodyA = m_joint1->GetBodyB();

	// Get geometry of joint1
	Transform2D xfA = m_bodyA->m_xf;
	real32 aA = m_bodyA->m_sweep.a;
	Transform2D xfC = m_bodyC->m_xf;
	real32 aC = m_bodyC->m_sweep.a;

	if (m_typeA == revoluteJoint)
	{
		RevoluteJoint* revolute = (RevoluteJoint*)def->joint1;
		m_localAnchorC = revolute->m_localAnchorA;
		m_localAnchorA = revolute->m_localAnchorB;
		m_referenceAngleA = revolute->m_referenceAngle;
		m_localAxisC = glm::vec2(0,0);

		coordinateA = aA - aC - m_referenceAngleA;
	}
	else
	{
		PrismaticJoint* prismatic = (PrismaticJoint*)def->joint1;
		m_localAnchorC = prismatic->m_localAnchorA;
		m_localAnchorA = prismatic->m_localAnchorB;
		m_referenceAngleA = prismatic->m_referenceAngle;
		m_localAxisC = prismatic->m_localXAxisA;

		glm::vec2 pC = m_localAnchorC;
		glm::vec2 pA = MathUtils::MulT(xfC.q, MathUtils::Mul(xfA.q, m_localAnchorA) + (xfA.p - xfC.p));
		coordinateA = glm::dot(pA - pC, m_localAxisC);
	}

	m_bodyD = m_joint2->GetBodyA();
	m_bodyB = m_joint2->GetBodyB();

	// Get geometry of joint2
	Transform2D xfB = m_bodyB->m_xf;
	real32 aB = m_bodyB->m_sweep.a;
	Transform2D xfD = m_bodyD->m_xf;
	real32 aD = m_bodyD->m_sweep.a;

	if (m_typeB == revoluteJoint)
	{
		RevoluteJoint* revolute = (RevoluteJoint*)def->joint2;
		m_localAnchorD = revolute->m_localAnchorA;
		m_localAnchorB = revolute->m_localAnchorB;
		m_referenceAngleB = revolute->m_referenceAngle;
		m_localAxisD = glm::vec2(0,0);

		coordinateB = aB - aD - m_referenceAngleB;
	}
	else
	{
		PrismaticJoint* prismatic = (PrismaticJoint*)def->joint2;
		m_localAnchorD = prismatic->m_localAnchorA;
		m_localAnchorB = prismatic->m_localAnchorB;
		m_referenceAngleB = prismatic->m_referenceAngle;
		m_localAxisD = prismatic->m_localXAxisA;

		glm::vec2 pD = m_localAnchorD;
		glm::vec2 pB = MathUtils::MulT(xfD.q, MathUtils::Mul(xfB.q, m_localAnchorB) + (xfB.p - xfD.p));
		coordinateB = glm::dot(pB - pD, m_localAxisD);
	}

	m_ratio = def->ratio;

	m_constant = coordinateA + m_ratio * coordinateB;

	m_impulse = 0.0f;
}

void GearJoint::InitVelocityConstraints(const SolverData& data)
{
	m_indexA = m_bodyA->m_islandIndex;
	m_indexB = m_bodyB->m_islandIndex;
	m_indexC = m_bodyC->m_islandIndex;
	m_indexD = m_bodyD->m_islandIndex;
	m_lcA = m_bodyA->m_sweep.localCenter;
	m_lcB = m_bodyB->m_sweep.localCenter;
	m_lcC = m_bodyC->m_sweep.localCenter;
	m_lcD = m_bodyD->m_sweep.localCenter;
	m_mA = m_bodyA->m_invMass;
	m_mB = m_bodyB->m_invMass;
	m_mC = m_bodyC->m_invMass;
	m_mD = m_bodyD->m_invMass;
	m_iA = m_bodyA->m_invI;
	m_iB = m_bodyB->m_invI;
	m_iC = m_bodyC->m_invI;
	m_iD = m_bodyD->m_invI;

	real32 aA = data.positions[m_indexA].a;
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;

	real32 aB = data.positions[m_indexB].a;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	real32 aC = data.positions[m_indexC].a;
	glm::vec2 vC = data.velocities[m_indexC].v;
	real32 wC = data.velocities[m_indexC].w;

	real32 aD = data.positions[m_indexD].a;
	glm::vec2 vD = data.velocities[m_indexD].v;
	real32 wD = data.velocities[m_indexD].w;

	Rotation2D qA(aA), qB(aB), qC(aC), qD(aD);

	m_mass = 0.0f;

	if (m_typeA == revoluteJoint)
	{
		m_JvAC = glm::vec2(0,0);
		m_JwA = 1.0f;
		m_JwC = 1.0f;
		m_mass += m_iA + m_iC;
	}
	else
	{
		glm::vec2 u = MathUtils::Mul(qC, m_localAxisC);
		glm::vec2 rC = MathUtils::Mul(qC, m_localAnchorC - m_lcC);
		glm::vec2 rA = MathUtils::Mul(qA, m_localAnchorA - m_lcA);
		m_JvAC = u;
		m_JwC = MathUtils::Cross2(rC, u);
		m_JwA = MathUtils::Cross2(rA, u);
		m_mass += m_mC + m_mA + m_iC * m_JwC * m_JwC + m_iA * m_JwA * m_JwA;
	}

	if (m_typeB == revoluteJoint)
	{
		m_JvBD = glm::vec2(0,0);
		m_JwB = m_ratio;
		m_JwD = m_ratio;
		m_mass += m_ratio * m_ratio * (m_iB + m_iD);
	}
	else
	{
		glm::vec2 u = MathUtils::Mul(qD, m_localAxisD);
		glm::vec2 rD = MathUtils::Mul(qD, m_localAnchorD - m_lcD);
		glm::vec2 rB = MathUtils::Mul(qB, m_localAnchorB - m_lcB);
		m_JvBD = m_ratio * u;
		m_JwD = m_ratio * MathUtils::Cross2(rD, u);
		m_JwB = m_ratio * MathUtils::Cross2(rB, u);
		m_mass += m_ratio * m_ratio * (m_mD + m_mB) + m_iD * m_JwD * m_JwD + m_iB * m_JwB * m_JwB;
	}

	// Compute effective mass.
	m_mass = m_mass > 0.0f ? 1.0f / m_mass : 0.0f;

	if (data.step.warmStarting)
	{
		vA += (m_mA * m_impulse) * m_JvAC;
		wA += m_iA * m_impulse * m_JwA;
		vB += (m_mB * m_impulse) * m_JvBD;
		wB += m_iB * m_impulse * m_JwB;
		vC -= (m_mC * m_impulse) * m_JvAC;
		wC -= m_iC * m_impulse * m_JwC;
		vD -= (m_mD * m_impulse) * m_JvBD;
		wD -= m_iD * m_impulse * m_JwD;
	}
	else
	{
		m_impulse = 0.0f;
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
	data.velocities[m_indexC].v = vC;
	data.velocities[m_indexC].w = wC;
	data.velocities[m_indexD].v = vD;
	data.velocities[m_indexD].w = wD;
}

void GearJoint::SolveVelocityConstraints(const SolverData& data)
{
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;
	glm::vec2 vC = data.velocities[m_indexC].v;
	real32 wC = data.velocities[m_indexC].w;
	glm::vec2 vD = data.velocities[m_indexD].v;
	real32 wD = data.velocities[m_indexD].w;

	real32 Cdot = glm::dot(m_JvAC, vA - vC) + glm::dot(m_JvBD, vB - vD);
	Cdot += (m_JwA * wA - m_JwC * wC) + (m_JwB * wB - m_JwD * wD);

	real32 impulse = -m_mass * Cdot;
	m_impulse += impulse;

	vA += (m_mA * impulse) * m_JvAC;
	wA += m_iA * impulse * m_JwA;
	vB += (m_mB * impulse) * m_JvBD;
	wB += m_iB * impulse * m_JwB;
	vC -= (m_mC * impulse) * m_JvAC;
	wC -= m_iC * impulse * m_JwC;
	vD -= (m_mD * impulse) * m_JvBD;
	wD -= m_iD * impulse * m_JwD;

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
	data.velocities[m_indexC].v = vC;
	data.velocities[m_indexC].w = wC;
	data.velocities[m_indexD].v = vD;
	data.velocities[m_indexD].w = wD;
}

bool GearJoint::SolvePositionConstraints(const SolverData& data)
{
	glm::vec2 cA = data.positions[m_indexA].c;
	real32 aA = data.positions[m_indexA].a;
	glm::vec2 cB = data.positions[m_indexB].c;
	real32 aB = data.positions[m_indexB].a;
	glm::vec2 cC = data.positions[m_indexC].c;
	real32 aC = data.positions[m_indexC].a;
	glm::vec2 cD = data.positions[m_indexD].c;
	real32 aD = data.positions[m_indexD].a;

	Rotation2D qA(aA), qB(aB), qC(aC), qD(aD);

	real32 linearError = 0.0f;

	real32 coordinateA, coordinateB;

	glm::vec2 JvAC, JvBD;
	real32 JwA, JwB, JwC, JwD;
	real32 mass = 0.0f;

	if (m_typeA == revoluteJoint)
	{
		JvAC = glm::vec2(0,0);
		JwA = 1.0f;
		JwC = 1.0f;
		mass += m_iA + m_iC;

		coordinateA = aA - aC - m_referenceAngleA;
	}
	else
	{
		glm::vec2 u = MathUtils::Mul(qC, m_localAxisC);
		glm::vec2 rC = MathUtils::Mul(qC, m_localAnchorC - m_lcC);
		glm::vec2 rA = MathUtils::Mul(qA, m_localAnchorA - m_lcA);
		JvAC = u;
		JwC = MathUtils::Cross2(rC, u);
		JwA = MathUtils::Cross2(rA, u);
		mass += m_mC + m_mA + m_iC * JwC * JwC + m_iA * JwA * JwA;

		glm::vec2 pC = m_localAnchorC - m_lcC;
		glm::vec2 pA = MathUtils::MulT(qC, rA + (cA - cC));
		coordinateA = glm::dot(pA - pC, m_localAxisC);
	}

	if (m_typeB == revoluteJoint)
	{
		JvBD = glm::vec2(0,0);
		JwB = m_ratio;
		JwD = m_ratio;
		mass += m_ratio * m_ratio * (m_iB + m_iD);

		coordinateB = aB - aD - m_referenceAngleB;
	}
	else
	{
		glm::vec2 u = MathUtils::Mul(qD, m_localAxisD);
		glm::vec2 rD = MathUtils::Mul(qD, m_localAnchorD - m_lcD);
		glm::vec2 rB = MathUtils::Mul(qB, m_localAnchorB - m_lcB);
		JvBD = m_ratio * u;
		JwD = m_ratio * MathUtils::Cross2(rD, u);
		JwB = m_ratio * MathUtils::Cross2(rB, u);
		mass += m_ratio * m_ratio * (m_mD + m_mB) + m_iD * JwD * JwD + m_iB * JwB * JwB;

		glm::vec2 pD = m_localAnchorD - m_lcD;
		glm::vec2 pB = MathUtils::MulT(qD, rB + (cB - cD));
		coordinateB = glm::dot(pB - pD, m_localAxisD);
	}

	real32 C = (coordinateA + m_ratio * coordinateB) - m_constant;

	real32 impulse = 0.0f;
	if (mass > 0.0f)
	{
		impulse = -C / mass;
	}

	cA += m_mA * impulse * JvAC;
	aA += m_iA * impulse * JwA;
	cB += m_mB * impulse * JvBD;
	aB += m_iB * impulse * JwB;
	cC -= m_mC * impulse * JvAC;
	aC -= m_iC * impulse * JwC;
	cD -= m_mD * impulse * JvBD;
	aD -= m_iD * impulse * JwD;

	data.positions[m_indexA].c = cA;
	data.positions[m_indexA].a = aA;
	data.positions[m_indexB].c = cB;
	data.positions[m_indexB].a = aB;
	data.positions[m_indexC].c = cC;
	data.positions[m_indexC].a = aC;
	data.positions[m_indexD].c = cD;
	data.positions[m_indexD].a = aD;

	// TODO_ERIN not implemented
	return linearError < linearSlop;
}

glm::vec2 GearJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

glm::vec2 GearJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

glm::vec2 GearJoint::GetReactionForce(real32 inv_dt) const
{
	glm::vec2 P = m_impulse * m_JvAC;
	return inv_dt * P;
}

real32 GearJoint::GetReactionTorque(real32 inv_dt) const
{
	real32 L = m_impulse * m_JwA;
	return inv_dt * L;
}

void GearJoint::SetRatio(real32 ratio)
{
	assert(MathUtils::IsVal(ratio));
	m_ratio = ratio;
}

real32 GearJoint::GetRatio() const
{
	return m_ratio;
}

void GearJoint::Dump()
{
	s32 indexA = m_bodyA->m_islandIndex;
	s32 indexB = m_bodyB->m_islandIndex;

	s32 index1 = m_joint1->m_index;
	s32 index2 = m_joint2->m_index;

	printf("  GearJointDef jd;\n");
	printf("  jd.bodyA = bodies[%d];\n", indexA);
	printf("  jd.bodyB = bodies[%d];\n", indexB);
	printf("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	printf("  jd.joint1 = joints[%d];\n", index1);
	printf("  jd.joint2 = joints[%d];\n", index2);
	printf("  jd.ratio = %.15lef;\n", m_ratio);
	printf("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}
