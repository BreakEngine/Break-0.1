#include "WheelJoint.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


// Linear constraint (point-to-line)
// d = pB - pA = xB + rB - xA - rA
// C = dot(ay, d)
// Cdot = dot(d, cross(wA, ay)) + dot(ay, vB + cross(wB, rB) - vA - cross(wA, rA))
//      = -dot(ay, vA) - dot(cross(d + rA, ay), wA) + dot(ay, vB) + dot(cross(rB, ay), vB)
// J = [-ay, -cross(d + rA, ay), ay, cross(rB, ay)]

// Spring linear constraint
// C = dot(ax, d)
// Cdot = = -dot(ax, vA) - dot(cross(d + rA, ax), wA) + dot(ax, vB) + dot(cross(rB, ax), vB)
// J = [-ax -cross(d+rA, ax) ax cross(rB, ax)]

// Motor rotational constraint
// Cdot = wB - wA
// J = [0 0 -1 0 0 1]

void WheelJointDef::Initialize(Body* bA, Body* bB, const glm::vec2& anchor, const glm::vec2& axis)
{
	bodyA = bA;
	bodyB = bB;
	localAnchorA = bodyA->GetLocalPoint(anchor);
	localAnchorB = bodyB->GetLocalPoint(anchor);
	localAxisA = bodyA->GetLocalVector(axis);
}

WheelJoint::WheelJoint(const WheelJointDef* def)
	: Joint(def)
{
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;
	m_localXAxisA = def->localAxisA;
	m_localYAxisA = MathUtils::Cross2(1.0f, m_localXAxisA);

	m_mass = 0.0f;
	m_impulse = 0.0f;
	m_motorMass = 0.0f;
	m_motorImpulse = 0.0f;
	m_springMass = 0.0f;
	m_springImpulse = 0.0f;

	m_maxMotorTorque = def->maxMotorTorque;
	m_motorSpeed = def->motorSpeed;
	m_enableMotor = def->enableMotor;

	m_frequencyHz = def->frequencyHz;
	m_dampingRatio = def->dampingRatio;

	m_bias = 0.0f;
	m_gamma = 0.0f;

	m_ax = glm::vec2(0,0);
	m_ay = glm::vec2(0,0);
}

void WheelJoint::InitVelocityConstraints(const SolverData& data)
{
	m_indexA = m_bodyA->m_islandIndex;
	m_indexB = m_bodyB->m_islandIndex;
	m_localCenterA = m_bodyA->m_sweep.localCenter;
	m_localCenterB = m_bodyB->m_sweep.localCenter;
	m_invMassA = m_bodyA->m_invMass;
	m_invMassB = m_bodyB->m_invMass;
	m_invIA = m_bodyA->m_invI;
	m_invIB = m_bodyB->m_invI;

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	glm::vec2 cA = data.positions[m_indexA].c;
	real32 aA = data.positions[m_indexA].a;
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;

	glm::vec2 cB = data.positions[m_indexB].c;
	real32 aB = data.positions[m_indexB].a;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	Rotation2D qA(aA), qB(aB);

	// Compute the effective masses.
	glm::vec2 rA = MathUtils::Mul(qA, m_localAnchorA - m_localCenterA);
	glm::vec2 rB = MathUtils::Mul(qB, m_localAnchorB - m_localCenterB);
	glm::vec2 d = cB + rB - cA - rA;

	// Point to line constraint
	{
		m_ay = MathUtils::Mul(qA, m_localYAxisA);
		m_sAy = MathUtils::Cross2(d + rA, m_ay);
		m_sBy = MathUtils::Cross2(rB, m_ay);

		m_mass = mA + mB + iA * m_sAy * m_sAy + iB * m_sBy * m_sBy;

		if (m_mass > 0.0f)
		{
			m_mass = 1.0f / m_mass;
		}
	}

	// Spring constraint
	m_springMass = 0.0f;
	m_bias = 0.0f;
	m_gamma = 0.0f;
	if (m_frequencyHz > 0.0f)
	{
		m_ax = MathUtils::Mul(qA, m_localXAxisA);
		m_sAx = MathUtils::Cross2(d + rA, m_ax);
		m_sBx = MathUtils::Cross2(rB, m_ax);

		real32 invMass = mA + mB + iA * m_sAx * m_sAx + iB * m_sBx * m_sBx;

		if (invMass > 0.0f)
		{
			m_springMass = 1.0f / invMass;

			real32 C = glm::dot(d, m_ax);

			// Frequency
			real32 omega = 2.0f * glm::pi<float>() * m_frequencyHz;

			// Damping coefficient
			real32 d = 2.0f * m_springMass * m_dampingRatio * omega;

			// Spring stiffness
			real32 k = m_springMass * omega * omega;

			// magic formulas
			real32 h = data.step.delta;
			m_gamma = h * (d + h * k);
			if (m_gamma > 0.0f)
			{
				m_gamma = 1.0f / m_gamma;
			}

			m_bias = C * h * k * m_gamma;

			m_springMass = invMass + m_gamma;
			if (m_springMass > 0.0f)
			{
				m_springMass = 1.0f / m_springMass;
			}
		}
	}
	else
	{
		m_springImpulse = 0.0f;
	}

	// Rotational motor
	if (m_enableMotor)
	{
		m_motorMass = iA + iB;
		if (m_motorMass > 0.0f)
		{
			m_motorMass = 1.0f / m_motorMass;
		}
	}
	else
	{
		m_motorMass = 0.0f;
		m_motorImpulse = 0.0f;
	}

	if (data.step.warmStarting)
	{
		// Account for variable time step.
		m_impulse *= data.step.dtRatio;
		m_springImpulse *= data.step.dtRatio;
		m_motorImpulse *= data.step.dtRatio;

		glm::vec2 P = m_impulse * m_ay + m_springImpulse * m_ax;
		real32 LA = m_impulse * m_sAy + m_springImpulse * m_sAx + m_motorImpulse;
		real32 LB = m_impulse * m_sBy + m_springImpulse * m_sBx + m_motorImpulse;

		vA -= m_invMassA * P;
		wA -= m_invIA * LA;

		vB += m_invMassB * P;
		wB += m_invIB * LB;
	}
	else
	{
		m_impulse = 0.0f;
		m_springImpulse = 0.0f;
		m_motorImpulse = 0.0f;
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

void WheelJoint::SolveVelocityConstraints(const SolverData& data)
{
	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	// Solve spring constraint
	{
		real32 Cdot = glm::dot(m_ax, vB - vA) + m_sBx * wB - m_sAx * wA;
		real32 impulse = -m_springMass * (Cdot + m_bias + m_gamma * m_springImpulse);
		m_springImpulse += impulse;

		glm::vec2 P = impulse * m_ax;
		real32 LA = impulse * m_sAx;
		real32 LB = impulse * m_sBx;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
	}

	// Solve rotational motor constraint
	{
		real32 Cdot = wB - wA - m_motorSpeed;
		real32 impulse = -m_motorMass * Cdot;

		real32 oldImpulse = m_motorImpulse;
		real32 maxImpulse = data.step.delta * m_maxMotorTorque;
		m_motorImpulse = glm::clamp(m_motorImpulse + impulse, -maxImpulse, maxImpulse);
		impulse = m_motorImpulse - oldImpulse;

		wA -= iA * impulse;
		wB += iB * impulse;
	}

	// Solve point to line constraint
	{
		real32 Cdot = glm::dot(m_ay, vB - vA) + m_sBy * wB - m_sAy * wA;
		real32 impulse = -m_mass * Cdot;
		m_impulse += impulse;

		glm::vec2 P = impulse * m_ay;
		real32 LA = impulse * m_sAy;
		real32 LB = impulse * m_sBy;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

bool WheelJoint::SolvePositionConstraints(const SolverData& data)
{
	glm::vec2 cA = data.positions[m_indexA].c;
	real32 aA = data.positions[m_indexA].a;
	glm::vec2 cB = data.positions[m_indexB].c;
	real32 aB = data.positions[m_indexB].a;

	Rotation2D qA(aA), qB(aB);

	glm::vec2 rA = MathUtils::Mul(qA, m_localAnchorA - m_localCenterA);
	glm::vec2 rB = MathUtils::Mul(qB, m_localAnchorB - m_localCenterB);
	glm::vec2 d = (cB - cA) + rB - rA;

	glm::vec2 ay = MathUtils::Mul(qA, m_localYAxisA);

	real32 sAy = MathUtils::Cross2(d + rA, ay);
	real32 sBy = MathUtils::Cross2(rB, ay);

	real32 C = glm::dot(d, ay);

	real32 k = m_invMassA + m_invMassB + m_invIA * m_sAy * m_sAy + m_invIB * m_sBy * m_sBy;

	real32 impulse;
	if (k != 0.0f)
	{
		impulse = - C / k;
	}
	else
	{
		impulse = 0.0f;
	}

	glm::vec2 P = impulse * ay;
	real32 LA = impulse * sAy;
	real32 LB = impulse * sBy;

	cA -= m_invMassA * P;
	aA -= m_invIA * LA;
	cB += m_invMassB * P;
	aB += m_invIB * LB;

	data.positions[m_indexA].c = cA;
	data.positions[m_indexA].a = aA;
	data.positions[m_indexB].c = cB;
	data.positions[m_indexB].a = aB;

	return glm::abs(C) <= linearSlop;
}

glm::vec2 WheelJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

glm::vec2 WheelJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

glm::vec2 WheelJoint::GetReactionForce(real32 inv_dt) const
{
	return inv_dt * (m_impulse * m_ay + m_springImpulse * m_ax);
}

real32 WheelJoint::GetReactionTorque(real32 inv_dt) const
{
	return inv_dt * m_motorImpulse;
}

real32 WheelJoint::GetJointTranslation() const
{
	Body* bA = m_bodyA;
	Body* bB = m_bodyB;

	glm::vec2 pA = bA->GetWorldPoint(m_localAnchorA);
	glm::vec2 pB = bB->GetWorldPoint(m_localAnchorB);
	glm::vec2 d = pB - pA;
	glm::vec2 axis = bA->GetWorldVector(m_localXAxisA);

	real32 translation = glm::dot(d, axis);
	return translation;
}

real32 WheelJoint::GetJointSpeed() const
{
	real32 wA = m_bodyA->m_angularVelocity;
	real32 wB = m_bodyB->m_angularVelocity;
	return wB - wA;
}

bool WheelJoint::IsMotorEnabled() const
{
	return m_enableMotor;
}

void WheelJoint::EnableMotor(bool flag)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_enableMotor = flag;
}

void WheelJoint::SetMotorSpeed(real32 speed)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_motorSpeed = speed;
}

void WheelJoint::SetMaxMotorTorque(real32 torque)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_maxMotorTorque = torque;
}

real32 WheelJoint::GetMotorTorque(real32 inv_dt) const
{
	return inv_dt * m_motorImpulse;
}

void WheelJoint::Dump()
{
	s32 indexA = m_bodyA->m_islandIndex;
	s32 indexB = m_bodyB->m_islandIndex;

	printf("  WheelJointDef jd;\n");
	printf("  jd.bodyA = bodies[%d];\n", indexA);
	printf("  jd.bodyB = bodies[%d];\n", indexB);
	printf("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	printf("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	printf("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	printf("  jd.localAxisA.Set(%.15lef, %.15lef);\n", m_localXAxisA.x, m_localXAxisA.y);
	printf("  jd.enableMotor = bool(%d);\n", m_enableMotor);
	printf("  jd.motorSpeed = %.15lef;\n", m_motorSpeed);
	printf("  jd.maxMotorTorque = %.15lef;\n", m_maxMotorTorque);
	printf("  jd.frequencyHz = %.15lef;\n", m_frequencyHz);
	printf("  jd.dampingRatio = %.15lef;\n", m_dampingRatio);
	printf("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}
