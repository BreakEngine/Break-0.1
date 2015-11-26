#include "RevoluteJoint.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;



// Point-to-point constraint
// C = p2 - p1
// Cdot = v2 - v1
//      = v2 + MathUtils::Cross2(w2, r2) - v1 - MathUtils::Cross2(w1, r1)
// J = [-I -r1_skew I r2_skew ]
// Identity used:
// w k % (rx i + ry j) = w * (-ry i + rx j)

// Motor constraint
// Cdot = w2 - w1
// J = [0 0 -1 0 0 1]
// K = invI1 + invI2

void RevoluteJointDef::Initialize(Body* bA, Body* bB, const glm::vec2& anchor)
{
	bodyA = bA;
	bodyB = bB;
	localAnchorA = bodyA->GetLocalPoint(anchor);
	localAnchorB = bodyB->GetLocalPoint(anchor);
	referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
}

RevoluteJoint::RevoluteJoint(const RevoluteJointDef* def)
	: Joint(def)
{
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;
	m_referenceAngle = def->referenceAngle;

	m_impulse = glm::vec3(0,0,0);
	m_motorImpulse = 0.0f;

	m_lowerAngle = def->lowerAngle;
	m_upperAngle = def->upperAngle;
	m_maxMotorTorque = def->maxMotorTorque;
	m_motorSpeed = def->motorSpeed;
	m_enableLimit = def->enableLimit;
	m_enableMotor = def->enableMotor;
	m_limitState = inactiveLimit;
}

void RevoluteJoint::InitVelocityConstraints(const SolverData& data)
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

	bool fixedRotation = (iA + iB == 0.0f);

	//m_mass.ex.x = mA + mB + m_rA.y * m_rA.y * iA + m_rB.y * m_rB.y * iB;
	//m_mass.ey.x = -m_rA.y * m_rA.x * iA - m_rB.y * m_rB.x * iB;
	//m_mass.ez.x = -m_rA.y * iA - m_rB.y * iB;
	//m_mass.ex.y = m_mass.ey.x;
	//m_mass.ey.y = mA + mB + m_rA.x * m_rA.x * iA + m_rB.x * m_rB.x * iB;
	//m_mass.ez.y = m_rA.x * iA + m_rB.x * iB;
	//m_mass.ex.z = m_mass.ez.x;
	//m_mass.ey.z = m_mass.ez.y;
	//m_mass.ez.z = iA + iB;

	//test here ((shaalan))
	m_mass[0][0] = mA + mB + m_rA.y * m_rA.y * iA + m_rB.y * m_rB.y * iB;
	m_mass[1][0] = -m_rA.y * m_rA.x * iA - m_rB.y * m_rB.x * iB;
	m_mass[2][0] = -m_rA.y * iA - m_rB.y * iB;
	m_mass[0][1] = m_mass[1][0];
	m_mass[1][1] = mA + mB + m_rA.x * m_rA.x * iA + m_rB.x * m_rB.x * iB;
	m_mass[2][1] = m_rA.x * iA + m_rB.x * iB;
	m_mass[0][2] = m_mass[2][0];
	m_mass[1][2] = m_mass[2][1];
	m_mass[2][2] = iA + iB;

	m_motorMass = iA + iB;
	if (m_motorMass > 0.0f)
	{
		m_motorMass = 1.0f / m_motorMass;
	}

	if (m_enableMotor == false || fixedRotation)
	{
		m_motorImpulse = 0.0f;
	}

	if (m_enableLimit && fixedRotation == false)
	{
		real32 jointAngle = aB - aA - m_referenceAngle;
		if (glm::abs(m_upperAngle - m_lowerAngle) < 2.0f * angularSlop)
		{
			m_limitState = equalLimits;
		}
		else if (jointAngle <= m_lowerAngle)
		{
			if (m_limitState != atLowerLimit)
			{
				m_impulse.z = 0.0f;
			}
			m_limitState = atLowerLimit;
		}
		else if (jointAngle >= m_upperAngle)
		{
			if (m_limitState != atUpperLimit)
			{
				m_impulse.z = 0.0f;
			}
			m_limitState = atUpperLimit;
		}
		else
		{
			m_limitState = inactiveLimit;
			m_impulse.z = 0.0f;
		}
	}
	else
	{
		m_limitState = inactiveLimit;
	}

	if (data.step.warmStarting)
	{
		// Scale impulses to support a variable time step.
		m_impulse *= data.step.dtRatio;
		m_motorImpulse *= data.step.dtRatio;

		glm::vec2 P(m_impulse.x, m_impulse.y);
		
		vA -= mA * P;
		wA -= iA * (MathUtils::Cross2(m_rA, P) + m_motorImpulse + m_impulse.z);

		vB += mB * P;
		wB += iB * (MathUtils::Cross2(m_rB, P) + m_motorImpulse + m_impulse.z);
	}
	else
	{
		m_impulse = glm::vec3(0,0,0);
		m_motorImpulse = 0.0f;
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

void RevoluteJoint::SolveVelocityConstraints(const SolverData& data)
{
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	bool fixedRotation = (iA + iB == 0.0f);

	// Solve motor constraint.
	if (m_enableMotor && m_limitState != equalLimits && fixedRotation == false)
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

	// Solve limit constraint.
	if (m_enableLimit && m_limitState != inactiveLimit && fixedRotation == false)
	{
		glm::vec2 Cdot1 = vB + MathUtils::Cross2(wB, m_rB) - vA - MathUtils::Cross2(wA, m_rA);
		real32 Cdot2 = wB - wA;
		glm::vec3 Cdot(Cdot1.x, Cdot1.y, Cdot2);

		//glm::vec3 impulse = -m_mass.Solve33(Cdot);  <<<========= test here ((shaalan))
		glm::vec3 impulse = MathUtils::SolveEq3(-m_mass , Cdot);


		if (m_limitState == equalLimits)
		{
			m_impulse += impulse;
		}
		else if (m_limitState == atLowerLimit)
		{
			real32 newImpulse = m_impulse.z + impulse.z;
			if (newImpulse < 0.0f)
			{
				glm::vec2 rhs = -Cdot1 + m_impulse.z * glm::vec2(m_mass[2][0], m_mass[2][1]);

				//glm::vec2 reduced = m_mass.Solve22(rhs); <<<========= test here ((shaalan))
				glm::vec2 reduced = MathUtils::SolveEq2(m_mass , rhs);

				impulse.x = reduced.x;
				impulse.y = reduced.y;
				impulse.z = -m_impulse.z;
				m_impulse.x += reduced.x;
				m_impulse.y += reduced.y;
				m_impulse.z = 0.0f;
			}
			else
			{
				m_impulse += impulse;
			}
		}
		else if (m_limitState == atUpperLimit)
		{
			real32 newImpulse = m_impulse.z + impulse.z;
			if (newImpulse > 0.0f)
			{
				glm::vec2 rhs = -Cdot1 + m_impulse.z * glm::vec2(m_mass[2][0], m_mass[2][1]);

				//glm::vec2 reduced = m_mass.Solve22(rhs); <<<========= test here ((shaalan))
				glm::vec2 reduced = MathUtils::SolveEq2(m_mass , rhs);

				impulse.x = reduced.x;
				impulse.y = reduced.y;
				impulse.z = -m_impulse.z;
				m_impulse.x += reduced.x;
				m_impulse.y += reduced.y;
				m_impulse.z = 0.0f;
			}
			else
			{
				m_impulse += impulse;
			}
		}

		glm::vec2 P(impulse.x, impulse.y);

		vA -= mA * P;
		wA -= iA * (MathUtils::Cross2(m_rA, P) + impulse.z);

		vB += mB * P;
		wB += iB * (MathUtils::Cross2(m_rB, P) + impulse.z);
	}
	else
	{
		// Solve point-to-point constraint
		glm::vec2 Cdot = vB + MathUtils::Cross2(wB, m_rB) - vA - MathUtils::Cross2(wA, m_rA);

		//glm::vec2 impulse = m_mass.Solve22(-Cdot); <<<========= test here ((shaalan))
		glm::vec2 impulse = MathUtils::SolveEq2(m_mass,-Cdot);

		m_impulse.x += impulse.x;
		m_impulse.y += impulse.y;

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

bool RevoluteJoint::SolvePositionConstraints(const SolverData& data)
{
	glm::vec2 cA = data.positions[m_indexA].c;
	real32 aA = data.positions[m_indexA].a;
	glm::vec2 cB = data.positions[m_indexB].c;
	real32 aB = data.positions[m_indexB].a;

	Rotation2D qA(aA), qB(aB);

	real32 angularError = 0.0f;
	real32 positionError = 0.0f;

	bool fixedRotation = (m_invIA + m_invIB == 0.0f);

	// Solve angular limit constraint.
	if (m_enableLimit && m_limitState != inactiveLimit && fixedRotation == false)
	{
		real32 angle = aB - aA - m_referenceAngle;
		real32 limitImpulse = 0.0f;

		if (m_limitState == equalLimits)
		{
			// Prevent large angular corrections
			real32 C = glm::clamp(angle - m_lowerAngle, -(real32) maxAngularCorrection,  (real32)maxAngularCorrection);
			limitImpulse = -m_motorMass * C;
			angularError = glm::abs(C);
		}
		else if (m_limitState == atLowerLimit)
		{
			real32 C = angle - m_lowerAngle;
			angularError = -C;

			// Prevent large angular corrections and allow some slop.
			C = glm::clamp(C +  (real32)angularSlop, - (real32)maxAngularCorrection, 0.0f);
			limitImpulse = -m_motorMass * C;
		}
		else if (m_limitState == atUpperLimit)
		{
			real32 C = angle - m_upperAngle;
			angularError = C;

			// Prevent large angular corrections and allow some slop.
			C = glm::clamp(C -  (real32)angularSlop, 0.0f, (real32)maxAngularCorrection);
			limitImpulse = -m_motorMass * C;
		}

		aA -= m_invIA * limitImpulse;
		aB += m_invIB * limitImpulse;
	}

	// Solve point-to-point constraint.
	{
		qA.Set(aA);
		qB.Set(aB);
		glm::vec2 rA = MathUtils::Mul(qA, m_localAnchorA - m_localCenterA);
		glm::vec2 rB = MathUtils::Mul(qB, m_localAnchorB - m_localCenterB);

		glm::vec2 C = cB + rB - cA - rA;
		positionError = C.length();

		real32 mA = m_invMassA, mB = m_invMassB;
		real32 iA = m_invIA, iB = m_invIB;

		glm::mat2 K;

		//K.ex.x = mA + mB + iA * rA.y * rA.y + iB * rB.y * rB.y;
		//K.ex.y = -iA * rA.x * rA.y - iB * rB.x * rB.y;
		//K.ey.x = K.ex.y;
		//K.ey.y = mA + mB + iA * rA.x * rA.x + iB * rB.x * rB.x;

		//test here ((shaalan))
		K[0][0] = mA + mB + iA * rA.y * rA.y + iB * rB.y * rB.y;
		K[0][1] = -iA * rA.x * rA.y - iB * rB.x * rB.y;
		K[1][0] = K[0][1];
		K[1][1] = mA + mB + iA * rA.x * rA.x + iB * rB.x * rB.x;

		//glm::vec2 impulse = -K.Solve(C);  test here ((shaalan))
		glm::vec2 impulse = MathUtils::SolveEq1(-K,C);

		cA -= mA * impulse;
		aA -= iA * MathUtils::Cross2(rA, impulse);

		cB += mB * impulse;
		aB += iB * MathUtils::Cross2(rB, impulse);
	}

	data.positions[m_indexA].c = cA;
	data.positions[m_indexA].a = aA;
	data.positions[m_indexB].c = cB;
	data.positions[m_indexB].a = aB;

	return positionError <= linearSlop && angularError <=  angularSlop;
}

glm::vec2 RevoluteJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

glm::vec2 RevoluteJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

glm::vec2 RevoluteJoint::GetReactionForce(real32 inv_dt) const
{
	glm::vec2 P(m_impulse.x, m_impulse.y);
	return inv_dt * P;
}

real32 RevoluteJoint::GetReactionTorque(real32 inv_dt) const
{
	return inv_dt * m_impulse.z;
}

real32 RevoluteJoint::GetJointAngle() const
{
	Body* bA = m_bodyA;
	Body* bB = m_bodyB;
	return bB->m_sweep.a - bA->m_sweep.a - m_referenceAngle;
}

real32 RevoluteJoint::GetJointSpeed() const
{
	Body* bA = m_bodyA;
	Body* bB = m_bodyB;
	return bB->m_angularVelocity - bA->m_angularVelocity;
}

bool RevoluteJoint::IsMotorEnabled() const
{
	return m_enableMotor;
}

void RevoluteJoint::EnableMotor(bool flag)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_enableMotor = flag;
}

real32 RevoluteJoint::GetMotorTorque(real32 inv_dt) const
{
	return inv_dt * m_motorImpulse;
}

void RevoluteJoint::SetMotorSpeed(real32 speed)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_motorSpeed = speed;
}

void RevoluteJoint::SetMaxMotorTorque(real32 torque)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_maxMotorTorque = torque;
}

bool RevoluteJoint::IsLimitEnabled() const
{
	return m_enableLimit;
}

void RevoluteJoint::EnableLimit(bool flag)
{
	if (flag != m_enableLimit)
	{
		m_bodyA->SetAwake(true);
		m_bodyB->SetAwake(true);
		m_enableLimit = flag;
		m_impulse.z = 0.0f;
	}
}

real32 RevoluteJoint::GetLowerLimit() const
{
	return m_lowerAngle;
}

real32 RevoluteJoint::GetUpperLimit() const
{
	return m_upperAngle;
}

void RevoluteJoint::SetLimits(real32 lower, real32 upper)
{
	assert(lower <= upper);

	if (lower != m_lowerAngle || upper != m_upperAngle)
	{
		m_bodyA->SetAwake(true);
		m_bodyB->SetAwake(true);
		m_impulse.z = 0.0f;
		m_lowerAngle = lower;
		m_upperAngle = upper;
	}
}

void RevoluteJoint::Dump()
{
	s32 indexA = m_bodyA->m_islandIndex;
	s32 indexB = m_bodyB->m_islandIndex;

	printf("  RevoluteJointDef jd;\n");
	printf("  jd.bodyA = bodies[%d];\n", indexA);
	printf("  jd.bodyB = bodies[%d];\n", indexB);
	printf("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	printf("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	printf("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	printf("  jd.referenceAngle = %.15lef;\n", m_referenceAngle);
	printf("  jd.enableLimit = bool(%d);\n", m_enableLimit);
	printf("  jd.lowerAngle = %.15lef;\n", m_lowerAngle);
	printf("  jd.upperAngle = %.15lef;\n", m_upperAngle);
	printf("  jd.enableMotor = bool(%d);\n", m_enableMotor);
	printf("  jd.motorSpeed = %.15lef;\n", m_motorSpeed);
	printf("  jd.maxMotorTorque = %.15lef;\n", m_maxMotorTorque);
	printf("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}
