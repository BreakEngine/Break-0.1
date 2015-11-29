#include "WeldJoint.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;


// Point-to-point constraint
// C = p2 - p1
// Cdot = v2 - v1
//      = v2 + MathUtils::Cross2(w2, r2) - v1 - MathUtils::Cross2(w1, r1)
// J = [-I -r1_skew I r2_skew ]
// Identity used:
// w k % (rx i + ry j) = w * (-ry i + rx j)

// Angle constraint
// C = angle2 - angle1 - referenceAngle
// Cdot = w2 - w1
// J = [0 0 -1 0 0 1]
// K = invI1 + invI2

void WeldJointDef::Initialize(Body* bA, Body* bB, const glm::vec2& anchor)
{
	bodyA = bA;
	bodyB = bB;
	localAnchorA = bodyA->GetLocalPoint(anchor);
	localAnchorB = bodyB->GetLocalPoint(anchor);
	referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
}

WeldJoint::WeldJoint(const WeldJointDef* def)
	: Joint(def)
{
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;
	m_referenceAngle = def->referenceAngle;
	m_frequencyHz = def->frequencyHz;
	m_dampingRatio = def->dampingRatio;

	m_impulse = glm::vec3(0,0,0);
}

void WeldJoint::InitVelocityConstraints(const SolverData& data)
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

	m_rA = Rotation2D::Mul(qA, m_localAnchorA - m_localCenterA);
	m_rB = Rotation2D::Mul(qB, m_localAnchorB - m_localCenterB);

	// J = [-I -r1_skew I r2_skew]
	//     [ 0       -1 0       1]
	// r_skew = [-ry; rx]

	// Matlab
	// K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x,          -r1y*iA-r2y*iB]
	//     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB,           r1x*iA+r2x*iB]
	//     [          -r1y*iA-r2y*iB,           r1x*iA+r2x*iB,                   iA+iB]

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	glm::mat3 K;

	//K.ex.x = mA + mB + m_rA.y * m_rA.y * iA + m_rB.y * m_rB.y * iB;      test here ((shaalan))
	//K.ey.x = -m_rA.y * m_rA.x * iA - m_rB.y * m_rB.x * iB;
	//K.ez.x = -m_rA.y * iA - m_rB.y * iB;
	//K.ex.y = K.ey.x;
	//K.ey.y = mA + mB + m_rA.x * m_rA.x * iA + m_rB.x * m_rB.x * iB;
	//K.ez.y = m_rA.x * iA + m_rB.x * iB;
	//K.ex.z = K.ez.x;
	//K.ey.z = K.ez.y;
	//K.ez.z = iA + iB;

	K[0][0] = mA + mB + m_rA.y * m_rA.y * iA + m_rB.y * m_rB.y * iB;
	K[1][0] = -m_rA.y * m_rA.x * iA - m_rB.y * m_rB.x * iB;
	K[2][0] = -m_rA.y * iA - m_rB.y * iB;
	K[0][1] = K[1][0];
	K[1][1]= mA + mB + m_rA.x * m_rA.x * iA + m_rB.x * m_rB.x * iB;
	K[2][1] = m_rA.x * iA + m_rB.x * iB;
	K[0][2] = K[2][0];
	K[1][2] = K[2][1];
	K[2][2] = iA + iB;

	if (m_frequencyHz > 0.0f)
	{
		//K.GetInverse22(&m_mass);  //test here ((shaalan))
		K = MathUtils::GetInverseEq2(K,m_mass);


		real32 invM = iA + iB;
		real32 m = invM > 0.0f ? 1.0f / invM : 0.0f;

		real32 C = aB - aA - m_referenceAngle;

		// Frequency
		real32 omega = 2.0f * glm::pi<float>()* m_frequencyHz;

		// Damping coefficient
		real32 d = 2.0f * m * m_dampingRatio * omega;

		// Spring stiffness
		real32 k = m * omega * omega;

		// magic formulas
		real32 h = data.step.delta;
		m_gamma = h * (d + h * k);
		m_gamma = m_gamma != 0.0f ? 1.0f / m_gamma : 0.0f;
		m_bias = C * h * k * m_gamma;

		invM += m_gamma;
		m_mass[2][2] = invM != 0.0f ? 1.0f / invM : 0.0f;
	}
	else
	{
		//K.GetSymInverse33(&m_mass);   test here ((shaalan))
		MathUtils::GetSymInverseEq3(K,m_mass);

		m_gamma = 0.0f;
		m_bias = 0.0f;
	}

	if (data.step.warmStarting)
	{
		// Scale impulses to support a variable time step.
		m_impulse *= data.step.dtRatio;

		glm::vec2 P(m_impulse.x, m_impulse.y);

		vA -= mA * P;
		wA -= iA * (MathUtils::Cross2(m_rA, P) + m_impulse.z);

		vB += mB * P;
		wB += iB * (MathUtils::Cross2(m_rB, P) + m_impulse.z);
	}
	else
	{
		m_impulse = glm::vec3(0,0,0);
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

void WeldJoint::SolveVelocityConstraints(const SolverData& data)
{
	glm::vec2 vA = data.velocities[m_indexA].v;
	real32 wA = data.velocities[m_indexA].w;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	if (m_frequencyHz > 0.0f)
	{
		real32 Cdot2 = wB - wA;

		real32 impulse2 = -m_mass[2][2] * (Cdot2 + m_bias + m_gamma * m_impulse.z);
		m_impulse.z += impulse2;

		wA -= iA * impulse2;
		wB += iB * impulse2;

		glm::vec2 Cdot1 = vB + MathUtils::Cross2(wB, m_rB) - vA - MathUtils::Cross2(wA, m_rA);

		glm::vec2 impulse1 = -MathUtils::Mul(m_mass, Cdot1);
		m_impulse.x += impulse1.x;
		m_impulse.y += impulse1.y;

		glm::vec2 P = impulse1;

		vA -= mA * P;
		wA -= iA * MathUtils::Cross2(m_rA, P);

		vB += mB * P;
		wB += iB * MathUtils::Cross2(m_rB, P);
	}
	else
	{
		glm::vec2 Cdot1 = vB + MathUtils::Cross2(wB, m_rB) - vA - MathUtils::Cross2(wA, m_rA);
		real32 Cdot2 = wB - wA;
		glm::vec3 Cdot(Cdot1.x, Cdot1.y, Cdot2);

		glm::vec3 impulse = -(m_mass * Cdot);
		m_impulse += impulse;

		glm::vec2 P(impulse.x, impulse.y);

		vA -= mA * P;
		wA -= iA * (MathUtils::Cross2(m_rA, P) + impulse.z);

		vB += mB * P;
		wB += iB * (MathUtils::Cross2(m_rB, P) + impulse.z);
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

bool WeldJoint::SolvePositionConstraints(const SolverData& data)
{
	glm::vec2 cA = data.positions[m_indexA].c;
	real32 aA = data.positions[m_indexA].a;
	glm::vec2 cB = data.positions[m_indexB].c;
	real32 aB = data.positions[m_indexB].a;

	Rotation2D qA(aA), qB(aB);

	real32 mA = m_invMassA, mB = m_invMassB;
	real32 iA = m_invIA, iB = m_invIB;

	glm::vec2 rA = Rotation2D::Mul(qA, m_localAnchorA - m_localCenterA);
	glm::vec2 rB = Rotation2D::Mul(qB, m_localAnchorB - m_localCenterB);

	real32 positionError, angularError;

	glm::mat3 K;
	//<<<<<<<=============== test here ((shaalan))

	//K.ex.x = mA + mB + rA.y * rA.y * iA + rB.y * rB.y * iB;
	//K.ey.x = -rA.y * rA.x * iA - rB.y * rB.x * iB;
	//K.ez.x = -rA.y * iA - rB.y * iB;
	//K.ex.y = K.ey.x;
	//K.ey.y = mA + mB + rA.x * rA.x * iA + rB.x * rB.x * iB;
	//K.ez.y = rA.x * iA + rB.x * iB;
	//K.ex.z = K.ez.x;
	//K.ey.z = K.ez.y;
	//K.ez.z = iA + iB;

	K[0][0] = mA + mB + rA.y * rA.y * iA + rB.y * rB.y * iB;
	K[1][0] = -rA.y * rA.x * iA - rB.y * rB.x * iB;
	K[2][0] = -rA.y * iA - rB.y * iB;
	K[0][1] = K[1][0];
	K[1][1] = mA + mB + rA.x * rA.x * iA + rB.x * rB.x * iB;
	K[2][1] = rA.x * iA + rB.x * iB;
	K[0][2] = K[2][0];
	K[1][2] = K[2][1];
	K[2][2] = iA + iB;

	if (m_frequencyHz > 0.0f)
	{
		glm::vec2 C1 =  cB + rB - cA - rA;

		positionError = C1.length();
		angularError = 0.0f;

		//glm::vec2 P = -K.Solve22(C1);
		glm::vec2 P = MathUtils::SolveEq2(-K,C1);

		cA -= mA * P;
		aA -= iA * MathUtils::Cross2(rA, P);

		cB += mB * P;
		aB += iB * MathUtils::Cross2(rB, P);
	}
	else
	{
		glm::vec2 C1 =  cB + rB - cA - rA;
		real32 C2 = aB - aA - m_referenceAngle;

		positionError = C1.length();
		angularError = glm::abs(C2);

		glm::vec3 C(C1.x, C1.y, C2);

		//glm::vec3 impulse = -K.Solve33(C);
		glm::vec3 impulse = MathUtils::SolveEq3(-K,C);


		glm::vec2 P(impulse.x, impulse.y);

		cA -= mA * P;
		aA -= iA * (MathUtils::Cross2(rA, P) + impulse.z);

		cB += mB * P;
		aB += iB * (MathUtils::Cross2(rB, P) + impulse.z);
	}

	data.positions[m_indexA].c = cA;
	data.positions[m_indexA].a = aA;
	data.positions[m_indexB].c = cB;
	data.positions[m_indexB].a = aB;

	return positionError <= linearSlop && angularError <= angularSlop;
}

glm::vec2 WeldJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

glm::vec2 WeldJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

glm::vec2 WeldJoint::GetReactionForce(real32 inv_dt) const
{
	glm::vec2 P(m_impulse.x, m_impulse.y);
	return inv_dt * P;
}

real32 WeldJoint::GetReactionTorque(real32 inv_dt) const
{
	return inv_dt * m_impulse.z;
}

void WeldJoint::Dump()
{
	s32 indexA = m_bodyA->m_islandIndex;
	s32 indexB = m_bodyB->m_islandIndex;

	printf("  WeldJointDef jd;\n");
	printf("  jd.bodyA = bodies[%d];\n", indexA);
	printf("  jd.bodyB = bodies[%d];\n", indexB);
	printf("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	printf("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	printf("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	printf("  jd.referenceAngle = %.15lef;\n", m_referenceAngle);
	printf("  jd.frequencyHz = %.15lef;\n", m_frequencyHz);
	printf("  jd.dampingRatio = %.15lef;\n", m_dampingRatio);
	printf("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}
