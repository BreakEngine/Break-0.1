#include "PrismaticJoint.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::physics;


// Linear constraint (point-to-line)
// d = p2 - p1 = x2 + r2 - x1 - r1
// C = dot(perp, d)
// Cdot = dot(d, cross(w1, perp)) + dot(perp, v2 + cross(w2, r2) - v1 - cross(w1, r1))
//      = -dot(perp, v1) - dot(cross(d + r1, perp), w1) + dot(perp, v2) + dot(cross(r2, perp), v2)
// J = [-perp, -cross(d + r1, perp), perp, cross(r2,perp)]
//
// Angular constraint
// C = a2 - a1 + a_initial
// Cdot = w2 - w1
// J = [0 0 -1 0 0 1]
//
// K = J * invM * JT
//
// J = [-a -s1 a s2]
//     [0  -1  0  1]
// a = perp
// s1 = cross(d + r1, a) = cross(p2 - x1, a)
// s2 = cross(r2, a) = cross(p2 - x2, a)


// Motor/Limit linear constraint
// C = dot(ax1, d)
// Cdot = = -dot(ax1, v1) - dot(cross(d + r1, ax1), w1) + dot(ax1, v2) + dot(cross(r2, ax1), v2)
// J = [-ax1 -cross(d+r1,ax1) ax1 cross(r2,ax1)]

// Block Solver
// We develop a block solver that includes the joint limit. This makes the limit stiff (inelastic) even
// when the mass has poor distribution (leading to large torques about the joint anchor points).
//
// The Jacobian has 3 rows:
// J = [-uT -s1 uT s2] // linear
//     [0   -1   0  1] // angular
//     [-vT -a1 vT a2] // limit
//
// u = perp
// v = axis
// s1 = cross(d + r1, u), s2 = cross(r2, u)
// a1 = cross(d + r1, v), a2 = cross(r2, v)

// M * (v2 - v1) = JT * df
// J * v2 = bias
//
// v2 = v1 + invM * JT * df
// J * (v1 + invM * JT * df) = bias
// K * df = bias - J * v1 = -Cdot
// K = J * invM * JT
// Cdot = J * v1 - bias
//
// Now solve for f2.
// df = f2 - f1
// K * (f2 - f1) = -Cdot
// f2 = invK * (-Cdot) + f1
//
// Clamp accumulated limit impulse.
// lower: f2(3) = max(f2(3), 0)
// upper: f2(3) = min(f2(3), 0)
//
// Solve for correct f2(1:2)
// K(1:2, 1:2) * f2(1:2) = -Cdot(1:2) - K(1:2,3) * f2(3) + K(1:2,1:3) * f1
//                       = -Cdot(1:2) - K(1:2,3) * f2(3) + K(1:2,1:2) * f1(1:2) + K(1:2,3) * f1(3)
// K(1:2, 1:2) * f2(1:2) = -Cdot(1:2) - K(1:2,3) * (f2(3) - f1(3)) + K(1:2,1:2) * f1(1:2)
// f2(1:2) = invK(1:2,1:2) * (-Cdot(1:2) - K(1:2,3) * (f2(3) - f1(3))) + f1(1:2)
//
// Now compute impulse to be applied:
// df = f2 - f1

void PrismaticJointDef::Initialize(Body* bA, Body* bB, const glm::vec2& anchor, const glm::vec2& axis)
{
	bodyA = bA;
	bodyB = bB;
	localAnchorA = bodyA->GetLocalPoint(anchor);
	localAnchorB = bodyB->GetLocalPoint(anchor);
	localAxisA = bodyA->GetLocalVector(axis);
	referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
}

PrismaticJoint::PrismaticJoint(const PrismaticJointDef* def)
	: Joint(def)
{
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;
	m_localXAxisA = def->localAxisA;
	m_localXAxisA = glm::normalize(m_localXAxisA);
	m_localYAxisA = MathUtils::Cross2(1.0f, m_localXAxisA);
	m_referenceAngle = def->referenceAngle;

	m_impulse = glm::vec3(0,0,0);
	m_motorMass = 0.0f;
	m_motorImpulse = 0.0f;

	m_lowerTranslation = def->lowerTranslation;
	m_upperTranslation = def->upperTranslation;
	m_maxMotorForce = def->maxMotorForce;
	m_motorSpeed = def->motorSpeed;
	m_enableLimit = def->enableLimit;
	m_enableMotor = def->enableMotor;
	m_limitState = inactiveLimit;

	m_axis = glm::vec2(0,0);
	m_perp = glm::vec2(0,0);
}

void PrismaticJoint::InitVelocityConstraints(const SolverData& data)
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

	// Compute the effective masses.
	glm::vec2 rA = MathUtils::Mul(qA, m_localAnchorA - m_localCenterA);
	glm::vec2 rB = MathUtils::Mul(qB, m_localAnchorB - m_localCenterB);
	glm::vec2 d = (cB - cA) + rB - rA;

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	// Compute motor Jacobian and effective mass.
	{
		m_axis = MathUtils::Mul(qA, m_localXAxisA);
		m_a1 = MathUtils::Cross2(d + rA, m_axis);
		m_a2 = MathUtils::Cross2(rB, m_axis);

		m_motorMass = mA + mB + iA * m_a1 * m_a1 + iB * m_a2 * m_a2;
		if (m_motorMass > 0.0f)
		{
			m_motorMass = 1.0f / m_motorMass;
		}
	}

	// Prismatic constraint.
	{
		m_perp = MathUtils::Mul(qA, m_localYAxisA);

		m_s1 = MathUtils::Cross2(d + rA, m_perp);
		m_s2 = MathUtils::Cross2(rB, m_perp);

		real32 k11 = mA + mB + iA * m_s1 * m_s1 + iB * m_s2 * m_s2;
		real32 k12 = iA * m_s1 + iB * m_s2;
		real32 k13 = iA * m_s1 * m_a1 + iB * m_s2 * m_a2;
		real32 k22 = iA + iB;
		if (k22 == 0.0f)
		{
			// For bodies with fixed rotation.
			k22 = 1.0f;
		}
		real32 k23 = iA * m_a1 + iB * m_a2;
		real32 k33 = mA + mB + iA * m_a1 * m_a1 + iB * m_a2 * m_a2;

		// to test here <<================== ((shaalan))
		//m_K.ex.Set(k11, k12, k13);
		m_K[0][0] = k11;
		m_K[0][1] = k12;
		m_K[0][2] = k13;

		//m_K.ey.Set(k12, k22, k23);
		m_K[1][0] = k12;
		m_K[1][1] = k22;
		m_K[1][2] = k23;

		//m_K.ez.Set(k13, k23, k33);
		m_K[2][0] = k13;
		m_K[2][1] = k23;
		m_K[2][2] = k33;
	}

	// Compute motor and limit terms.
	if (m_enableLimit)
	{
		real32 jointTranslation = glm::dot(m_axis, d);
		if (glm::abs(m_upperTranslation - m_lowerTranslation) < 2.0f * linearSlop)
		{
			m_limitState = equalLimits;
		}
		else if (jointTranslation <= m_lowerTranslation)
		{
			if (m_limitState != atLowerLimit)
			{
				m_limitState = atLowerLimit;
				m_impulse.z = 0.0f;
			}
		}
		else if (jointTranslation >= m_upperTranslation)
		{
			if (m_limitState != atUpperLimit)
			{
				m_limitState = atUpperLimit;
				m_impulse.z = 0.0f;
			}
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
		m_impulse.z = 0.0f;
	}

	if (m_enableMotor == false)
	{
		m_motorImpulse = 0.0f;
	}

	if (data.step.warmStarting)
	{
		// Account for variable time step.
		m_impulse *= data.step.dtRatio;
		m_motorImpulse *= data.step.dtRatio;

		glm::vec2 P = m_impulse.x * m_perp + (m_motorImpulse + m_impulse.z) * m_axis;
		real32 LA = m_impulse.x * m_s1 + m_impulse.y + (m_motorImpulse + m_impulse.z) * m_a1;
		real32 LB = m_impulse.x * m_s2 + m_impulse.y + (m_motorImpulse + m_impulse.z) * m_a2;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
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

void PrismaticJoint::SolveVelocityConstraints(const SolverData& data)
{
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	// Solve linear motor constraint.
	if (m_enableMotor && m_limitState != equalLimits)
	{
		real32 Cdot = glm::dot(m_axis, vB - vA) + m_a2 * wB - m_a1 * wA;
		real32 impulse = m_motorMass * (m_motorSpeed - Cdot);
		real32 oldImpulse = m_motorImpulse;
		real32 maxImpulse = data.step.delta * m_maxMotorForce;
		m_motorImpulse = glm::clamp(m_motorImpulse + impulse, -maxImpulse, maxImpulse);
		impulse = m_motorImpulse - oldImpulse;

		glm::vec2 P = impulse * m_axis;
		real32 LA = impulse * m_a1;
		real32 LB = impulse * m_a2;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
	}

	glm::vec2 Cdot1;
	Cdot1.x = glm::dot(m_perp, vB - vA) + m_s2 * wB - m_s1 * wA;
	Cdot1.y = wB - wA;

	if (m_enableLimit && m_limitState != inactiveLimit)
	{
		// Solve prismatic and limit constraint in block form.
		real32 Cdot2;
		Cdot2 = glm::dot(m_axis, vB - vA) + m_a2 * wB - m_a1 * wA;
		glm::vec3 Cdot(Cdot1.x, Cdot1.y, Cdot2);

		glm::vec3 f1 = m_impulse;

		//glm::vec3 df =  m_K.Solve33(-Cdot);  <<===== test here ((shaalan))
		glm::vec3 df =  MathUtils::SolveEq3(m_K , -Cdot);

		m_impulse += df;

		if (m_limitState == atLowerLimit)
		{
			m_impulse.z = glm::max(m_impulse.z, 0.0f);
		}
		else if (m_limitState == atUpperLimit)
		{
			m_impulse.z = glm::min(m_impulse.z, 0.0f);
		}

		// f2(1:2) = invK(1:2,1:2) * (-Cdot(1:2) - K(1:2,3) * (f2(3) - f1(3))) + f1(1:2)    <<===== test here ((shaalan))
		glm::vec2 b = -Cdot1 - (m_impulse.z - f1.z) * glm::vec2(m_K[2][0], m_K[2][1]);
		 
		//glm::vec2 f2r = m_K.Solve22(b) + glm::vec2(f1.x, f1.y);

		glm::vec2 f2r = MathUtils::SolveEq2(m_K,b) + glm::vec2(f1.x, f1.y);

		m_impulse.x = f2r.x;
		m_impulse.y = f2r.y;

		df = m_impulse - f1;

		glm::vec2 P = df.x * m_perp + df.z * m_axis;
		real32 LA = df.x * m_s1 + df.y + df.z * m_a1;
		real32 LB = df.x * m_s2 + df.y + df.z * m_a2;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
	}
	else
	{
		// Limit is inactive, just solve the prismatic constraint in block form.

		//glm::vec2 df = m_K.Solve22(-Cdot1);  test here ((shaalan))

		glm::vec2 df = MathUtils::SolveEq2(m_K,-Cdot1);
		m_impulse.x += df.x;
		m_impulse.y += df.y;

		glm::vec2 P = df.x * m_perp;
		real32 LA = df.x * m_s1 + df.y;
		real32 LB = df.x * m_s2 + df.y;

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

bool PrismaticJoint::SolvePositionConstraints(const SolverData& data)
{
	glm::vec2 cA = data.positions[m_indexA].c;
	real32 aA = data.positions[m_indexA].a;
	glm::vec2 cB = data.positions[m_indexB].c;
	real32 aB = data.positions[m_indexB].a;

	Rotation2D qA(aA), qB(aB);

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	// Compute fresh Jacobians
	glm::vec2 rA = MathUtils::Mul(qA, m_localAnchorA - m_localCenterA);
	glm::vec2 rB = MathUtils::Mul(qB, m_localAnchorB - m_localCenterB);
	glm::vec2 d = cB + rB - cA - rA;

	glm::vec2 axis = MathUtils::Mul(qA, m_localXAxisA);
	real32 a1 = MathUtils::Cross2(d + rA, axis);
	real32 a2 = MathUtils::Cross2(rB, axis);
	glm::vec2 perp = MathUtils::Mul(qA, m_localYAxisA);

	real32 s1 = MathUtils::Cross2(d + rA, perp);
	real32 s2 = MathUtils::Cross2(rB, perp);

	glm::vec3 impulse;
	glm::vec2 C1;
	C1.x = glm::dot(perp, d);
	C1.y = aB - aA - m_referenceAngle;

	real32 linearError = glm::abs(C1.x);
	real32 angularError = glm::abs(C1.y);

	bool active = false;
	real32 C2 = 0.0f;
	if (m_enableLimit)
	{
		real32 translation = glm::dot(axis, d);
		if (glm::abs(m_upperTranslation - m_lowerTranslation) < 2.0f * linearSlop)
		{
			// Prevent large angular corrections
			C2 = glm::clamp(translation, - maxLinearCorrection,  maxLinearCorrection);
			linearError = glm::max(linearError, glm::abs(translation));
			active = true;
		}
		else if (translation <= m_lowerTranslation)
		{
			// Prevent large linear corrections and allow some slop.
			C2 = glm::clamp(translation - m_lowerTranslation + linearSlop, - maxLinearCorrection, 0.0f);
			linearError = glm::max(linearError, m_lowerTranslation - translation);
			active = true;
		}
		else if (translation >= m_upperTranslation)
		{
			// Prevent large linear corrections and allow some slop.
			C2 = glm::clamp(translation - m_upperTranslation - linearSlop, 0.0f, maxLinearCorrection);
			linearError = glm::max(linearError, translation - m_upperTranslation);
			active = true;
		}
	}

	if (active)
	{
		real32 k11 = mA + mB + iA * s1 * s1 + iB * s2 * s2;
		real32 k12 = iA * s1 + iB * s2;
		real32 k13 = iA * s1 * a1 + iB * s2 * a2;
		real32 k22 = iA + iB;
		if (k22 == 0.0f)
		{
			// For fixed rotation
			k22 = 1.0f;
		}
		real32 k23 = iA * a1 + iB * a2;
		real32 k33 = mA + mB + iA * a1 * a1 + iB * a2 * a2;

		glm::mat3 K;
		//<<<===========================test here ((shaalan))
		//K.ex.Set(k11, k12, k13);
		//K.ey.Set(k12, k22, k23);
		//K.ez.Set(k13, k23, k33);

		K[0][0] = k11;
		K[0][1] = k12;
		K[0][2] = k13;

		K[1][0] = k12;
		K[1][1] = k22;
		K[1][2] = k23;

		K[2][0] = k13;
		K[2][1] = k23;
		K[2][2] = k33;


		glm::vec3 C;
		C.x = C1.x;
		C.y = C1.y;
		C.z = C2;

		//impulse = K.Solve33(-C);
		impulse = MathUtils::SolveEq3(K,-C);
	}
	else
	{
		real32 k11 = mA + mB + iA * s1 * s1 + iB * s2 * s2;
		real32 k12 = iA * s1 + iB * s2;
		real32 k22 = iA + iB;
		if (k22 == 0.0f)
		{
			k22 = 1.0f;
		}

		glm::mat2 K;


		//K.ex.Set(k11, k12);
		//K.ey.Set(k12, k22);   test here ((shaalan))

		K[0][0] = k11;
		K[0][1] = k12;

		K[1][0] = k12 ;
		K[1][1] = k22;



		//glm::vec2 impulse1 = K.Solve(-C1);
		glm::vec2 impulse1 = MathUtils::SolveEq1(K,-C1);

		impulse.x = impulse1.x;
		impulse.y = impulse1.y;
		impulse.z = 0.0f;
	}

	glm::vec2 P = impulse.x * perp + impulse.z * axis;
	real32 LA = impulse.x * s1 + impulse.y + impulse.z * a1;
	real32 LB = impulse.x * s2 + impulse.y + impulse.z * a2;

	cA -= mA * P;
	aA -= iA * LA;
	cB += mB * P;
	aB += iB * LB;

	data.positions[m_indexA].c = cA;
	data.positions[m_indexA].a = aA;
	data.positions[m_indexB].c = cB;
	data.positions[m_indexB].a = aB;

	return linearError <= linearSlop && angularError <= angularSlop;
}

glm::vec2 PrismaticJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

glm::vec2 PrismaticJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

glm::vec2 PrismaticJoint::GetReactionForce(real32 inv_dt) const
{
	return inv_dt * (m_impulse.x * m_perp + (m_motorImpulse + m_impulse.z) * m_axis);
}

real32 PrismaticJoint::GetReactionTorque(real32 inv_dt) const
{
	return inv_dt * m_impulse.y;
}

real32 PrismaticJoint::GetJointTranslation() const
{
	glm::vec2 pA = m_bodyA->GetWorldPoint(m_localAnchorA);
	glm::vec2 pB = m_bodyB->GetWorldPoint(m_localAnchorB);
	glm::vec2 d = pB - pA;
	glm::vec2 axis = m_bodyA->GetWorldVector(m_localXAxisA);

	real32 translation = glm::dot(d, axis);
	return translation;
}

real32 PrismaticJoint::GetJointSpeed() const
{
	Body* bA = m_bodyA;
	Body* bB = m_bodyB;

	glm::vec2 rA = MathUtils::Mul(bA->m_xf.q, m_localAnchorA - bA->m_sweep.localCenter);
	glm::vec2 rB = MathUtils::Mul(bB->m_xf.q, m_localAnchorB - bB->m_sweep.localCenter);
	glm::vec2 p1 = bA->m_sweep.c + rA;
	glm::vec2 p2 = bB->m_sweep.c + rB;
	glm::vec2 d = p2 - p1;
	glm::vec2 axis = MathUtils::Mul(bA->m_xf.q, m_localXAxisA);

	glm::vec2 vA = bA->m_linearVelocity;
	glm::vec2 vB = bB->m_linearVelocity;
	real32 wA = bA->m_angularVelocity;
	real32 wB = bB->m_angularVelocity;

	real32 speed = glm::dot(d, MathUtils::Cross2(wA, axis)) + glm::dot(axis, vB + MathUtils::Cross2(wB, rB) - vA - MathUtils::Cross2(wA, rA));
	return speed;
}

bool PrismaticJoint::IsLimitEnabled() const
{
	return m_enableLimit;
}

void PrismaticJoint::EnableLimit(bool flag)
{
	if (flag != m_enableLimit)
	{
		m_bodyA->SetAwake(true);
		m_bodyB->SetAwake(true);
		m_enableLimit = flag;
		m_impulse.z = 0.0f;
	}
}

real32 PrismaticJoint::GetLowerLimit() const
{
	return m_lowerTranslation;
}

real32 PrismaticJoint::GetUpperLimit() const
{
	return m_upperTranslation;
}

void PrismaticJoint::SetLimits(real32 lower, real32 upper)
{
	assert(lower <= upper);
	if (lower != m_lowerTranslation || upper != m_upperTranslation)
	{
		m_bodyA->SetAwake(true);
		m_bodyB->SetAwake(true);
		m_lowerTranslation = lower;
		m_upperTranslation = upper;
		m_impulse.z = 0.0f;
	}
}

bool PrismaticJoint::IsMotorEnabled() const
{
	return m_enableMotor;
}

void PrismaticJoint::EnableMotor(bool flag)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_enableMotor = flag;
}

void PrismaticJoint::SetMotorSpeed(real32 speed)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_motorSpeed = speed;
}

void PrismaticJoint::SetMaxMotorForce(real32 force)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_maxMotorForce = force;
}

real32 PrismaticJoint::GetMotorForce(real32 inv_dt) const
{
	return inv_dt * m_motorImpulse;
}

void PrismaticJoint::Dump()
{
	s32 indexA = m_bodyA->m_islandIndex;
	s32 indexB = m_bodyB->m_islandIndex;

	printf("  PrismaticJointDef jd;\n");
	printf("  jd.bodyA = bodies[%d];\n", indexA);
	printf("  jd.bodyB = bodies[%d];\n", indexB);
	printf("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	printf("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	printf("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	printf("  jd.localAxisA.Set(%.15lef, %.15lef);\n", m_localXAxisA.x, m_localXAxisA.y);
	printf("  jd.referenceAngle = %.15lef;\n", m_referenceAngle);
	printf("  jd.enableLimit = bool(%d);\n", m_enableLimit);
	printf("  jd.lowerTranslation = %.15lef;\n", m_lowerTranslation);
	printf("  jd.upperTranslation = %.15lef;\n", m_upperTranslation);
	printf("  jd.enableMotor = bool(%d);\n", m_enableMotor);
	printf("  jd.motorSpeed = %.15lef;\n", m_motorSpeed);
	printf("  jd.maxMotorForce = %.15lef;\n", m_maxMotorForce);
	printf("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}
