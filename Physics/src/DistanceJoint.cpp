#include "DistanceJoint.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;



// 1-D constrained system
// m (v2 - v1) = lambda
// v2 + (beta/h) * x1 + gamma * lambda = 0, gamma has units of inverse mass.
// x2 = x1 + h * v2

// 1-D mass-damper-spring system
// m (v2 - v1) + h * d * v2 + h * k * 

// C = norm(p2 - p1) - L
// u = (p2 - p1) / norm(p2 - p1)
// Cdot = dot(u, v2 + cross(w2, r2) - v1 - cross(w1, r1))
// J = [-u -cross(r1, u) u cross(r2, u)]
// K = J * invM * JT
//   = invMass1 + invI1 * cross(r1, u)^2 + invMass2 + invI2 * cross(r2, u)^2

void DistanceJointDef::Initialize(Body* b1, Body* b2 ,const glm::vec2& anchor1, const glm::vec2& anchor2)
{
	bodyA = b1;
	bodyB = b2;
	localAnchorA = bodyA->GetLocalPoint(anchor1);
	localAnchorB = bodyB->GetLocalPoint(anchor2);
	glm::vec2 d = anchor2 - anchor1;
	length = d.length();
}

DistanceJoint::DistanceJoint(const DistanceJointDef* def) : Joint(def)
{
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;
	m_length = def->length;
	m_frequencyHz = def->frequencyHz;
	m_dampingRatio = def->dampingRatio;
	m_impulse = 0.0f;
	m_gamma = 0.0f;
	m_bias = 0.0f;
}

void DistanceJoint::InitVelocityConstraints(const SolverData& data)
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

	m_rA = MathUtils::Mul(qA, m_localAnchorA - m_localCenterA);
	m_rB = MathUtils::Mul(qB, m_localAnchorB - m_localCenterB);
	m_u = cB + m_rB - cA - m_rA;

	// Handle singularity.
	real32 length = m_u.length();
	if (length > linearSlop)
	{
		m_u *= 1.0f / length;
	}
	else
	{
		m_u = glm::vec2(0.0f, 0.0f);
	}

	real32 crAu = MathUtils::Cross2(m_rA, m_u);
	real32 crBu = MathUtils::Cross2(m_rB, m_u);
	real32 invMass = m_invMassA + m_invIA * crAu * crAu + m_invMassB + m_invIB * crBu * crBu;

	// Compute the effective mass matrix.
	m_mass = invMass != 0.0f ? 1.0f / invMass : 0.0f;

	if (m_frequencyHz > 0.0f)
	{
		real32 C = length - m_length;

		// Frequency
		real32 omega = 2.0f * glm::pi<float>() * m_frequencyHz;

		// Damping coefficient
		real32 d = 2.0f * m_mass * m_dampingRatio * omega;

		// Spring stiffness
		real32 k = m_mass * omega * omega;

		// magic formulas
		real32 h = data.step.delta;
		m_gamma = h * (d + h * k);
		m_gamma = m_gamma != 0.0f ? 1.0f / m_gamma : 0.0f;
		m_bias = C * h * k * m_gamma;

		invMass += m_gamma;
		m_mass = invMass != 0.0f ? 1.0f / invMass : 0.0f;
	}
	else
	{
		m_gamma = 0.0f;
		m_bias = 0.0f;
	}

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

void DistanceJoint::SolveVelocityConstraints(const SolverData& data)
{
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	// Cdot = dot(u, v + cross(w, r))
	glm::vec2 vpA = vA + MathUtils::Cross2(wA, m_rA);
	glm::vec2 vpB = vB + MathUtils::Cross2(wB, m_rB);
	real32 Cdot = glm::dot(m_u, vpB - vpA);

	real32 impulse = -m_mass * (Cdot + m_bias + m_gamma * m_impulse);
	m_impulse += impulse;

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

bool DistanceJoint::SolvePositionConstraints(const SolverData& data)
{
	if (m_frequencyHz > 0.0f)
	{
		// There is no position correction for soft distance constraints.
		return true;
	}

	glm::vec2 cA = data.positions[m_indexA].c;
	real32 aA = data.positions[m_indexA].a;
	glm::vec2 cB = data.positions[m_indexB].c;
	real32 aB = data.positions[m_indexB].a;

	Rotation2D qA(aA), qB(aB);

	glm::vec2 rA = MathUtils::Mul(qA, m_localAnchorA - m_localCenterA);
	glm::vec2 rB = MathUtils::Mul(qB, m_localAnchorB - m_localCenterB);
	glm::vec2 u = cB + rB - cA - rA;

	real32 length = u.length();
	real32 C = length - m_length;
	C = glm::clamp(C, - maxLinearCorrection, maxLinearCorrection);

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

	return glm::abs(C) < linearSlop;
}

glm::vec2 DistanceJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

glm::vec2 DistanceJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

glm::vec2 DistanceJoint::GetReactionForce(real32 inv_dt) const
{
	glm::vec2 F = (inv_dt * m_impulse) * m_u;
	return F;
}

real32 DistanceJoint::GetReactionTorque(real32 inv_dt) const
{
	NOT_USED(inv_dt);
	return 0.0f;
}

void DistanceJoint::Dump()
{
	s32 indexA = m_bodyA->m_islandIndex;
	s32 indexB = m_bodyB->m_islandIndex;

	printf("  DistanceJointDef jd;\n");
	printf("  jd.bodyA = bodies[%d];\n", indexA);
	printf("  jd.bodyB = bodies[%d];\n", indexB);
	printf("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	printf("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	printf("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	printf("  jd.length = %.15lef;\n", m_length);
	printf("  jd.frequencyHz = %.15lef;\n", m_frequencyHz);
	printf("  jd.dampingRatio = %.15lef;\n", m_dampingRatio);
	printf("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}
