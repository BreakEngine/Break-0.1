#include "MouseJoint.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;


// p = attached point, m = mouse point
// C = p - m
// Cdot = v
//      = v + cross(w, r)
// J = [I r_skew]
// Identity used:
// w k % (rx i + ry j) = w * (-ry i + rx j)

MouseJoint::MouseJoint(const MouseJointDef* def) : Joint(def)
{
	assert( MathUtils::IsValid( def->target));
	assert(MathUtils::IsVal(def->maxForce) && def->maxForce >= 0.0f);
	assert(MathUtils::IsVal(def->frequencyHz) && def->frequencyHz >= 0.0f);
	assert(MathUtils::IsVal(def->dampingRatio) && def->dampingRatio >= 0.0f);

	m_targetA = def->target;
	m_localAnchorB = Transform2D::MulT(m_bodyB->GetTransform2D(), m_targetA);

	m_maxForce = def->maxForce;
	m_impulse = glm::vec2(0,0);

	m_frequencyHz = def->frequencyHz;
	m_dampingRatio = def->dampingRatio;

	m_beta = 0.0f;
	m_gamma = 0.0f;
}

void MouseJoint::SetTarget(const glm::vec2& target)
{
	if (m_bodyB->IsAwake() == false)
	{
		m_bodyB->SetAwake(true);
	}
	m_targetA = target;
}

const glm::vec2& MouseJoint::GetTarget() const
{
	return m_targetA;
}

void MouseJoint::SetMaxForce(real32 force)
{
	m_maxForce = force;
}

real32 MouseJoint::GetMaxForce() const
{
	return m_maxForce;
}

void MouseJoint::SetFrequency(real32 hz)
{
	m_frequencyHz = hz;
}

real32 MouseJoint::GetFrequency() const
{
	return m_frequencyHz;
}

void MouseJoint::SetDampingRatio(real32 ratio)
{
	m_dampingRatio = ratio;
}

real32 MouseJoint::GetDampingRatio() const
{
	return m_dampingRatio;
}

void MouseJoint::InitVelocityConstraints(const SolverData& data)
{
	m_indexB = m_bodyB->m_islandIndex;
	m_localCenterB = m_bodyB->m_sweep.localCenter;
	m_invMassB = m_bodyB->m_invMass;
	m_invIB = m_bodyB->m_invI;

	glm::vec2 cB = data.positions[m_indexB].c;
	real32 aB = data.positions[m_indexB].a;
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	Rotation2D qB(aB);

	real32 mass = m_bodyB->GetMass();

	// Frequency
	real32 omega = 2.0f * glm::pi<float>() * m_frequencyHz;

	// Damping coefficient
	real32 d = 2.0f * mass * m_dampingRatio * omega;

	// Spring stiffness
	real32 k = mass * (omega * omega);

	// magic formulas
	// gamma has units of inverse mass.
	// beta has units of inverse time.
	real32 h = data.step.delta;
	assert(d + h * k > FLT_EPSILON);
	m_gamma = h * (d + h * k);
	if (m_gamma != 0.0f)
	{
		m_gamma = 1.0f / m_gamma;
	}
	m_beta = h * k * m_gamma;

	// Compute the effective mass matrix.
	m_rB = Rotation2D::Mul(qB, m_localAnchorB - m_localCenterB);

	// K    = [(1/m1 + 1/m2) * eye(2) - skew(r1) * invI1 * skew(r1) - skew(r2) * invI2 * skew(r2)]
	//      = [1/m1+1/m2     0    ] + invI1 * [r1.y*r1.y -r1.x*r1.y] + invI2 * [r1.y*r1.y -r1.x*r1.y]
	//        [    0     1/m1+1/m2]           [-r1.x*r1.y r1.x*r1.x]           [-r1.x*r1.y r1.x*r1.x]
	glm::mat2 K;

	//K.ex.x = m_invMassB + m_invIB * m_rB.y * m_rB.y + m_gamma;
	//K.ex.y = -m_invIB * m_rB.x * m_rB.y;
	//K.ey.x = K.ex.y;
	//K.ey.y = m_invMassB + m_invIB * m_rB.x * m_rB.x + m_gamma;

	K[0][0] = m_invMassB + m_invIB * m_rB.y * m_rB.y + m_gamma;
	K[0][1] = -m_invIB * m_rB.x * m_rB.y;
	K[0][1] = K[0][1];
	K[1][1] = m_invMassB + m_invIB * m_rB.x * m_rB.x + m_gamma;

	m_mass = glm::inverse( K );

	m_C = cB + m_rB - m_targetA;
	m_C *= m_beta;

	// Cheat with some damping
	wB *= 0.98f;

	if (data.step.warmStarting)
	{
		m_impulse *= data.step.dtRatio;
		vB += m_invMassB * m_impulse;
		wB += m_invIB * MathUtils::Cross2(m_rB, m_impulse);
	}
	else
	{
		m_impulse = glm::vec2(0,0);
	}

	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

void MouseJoint::SolveVelocityConstraints(const SolverData& data)
{
	glm::vec2 vB = data.velocities[m_indexB].v;
	real32 wB = data.velocities[m_indexB].w;

	// Cdot = v + cross(w, r)
	glm::vec2 Cdot = vB + MathUtils::Cross2(wB, m_rB);

	//glm::vec2 impulse = Mul(m_mass, -(Cdot + m_C + m_gamma * m_impulse));
	glm::vec2 impulse = (m_mass * -(Cdot + m_C + m_gamma * m_impulse));

	glm::vec2 oldImpulse = m_impulse;
	m_impulse += impulse;
	real32 maxImpulse = data.step.delta * m_maxForce;

	if (MathUtils::LengthSquared( m_impulse ) > maxImpulse * maxImpulse)
	{
		m_impulse *= maxImpulse / m_impulse.length();
	}
	impulse = m_impulse - oldImpulse;

	vB += m_invMassB * impulse;
	wB += m_invIB * MathUtils::Cross2(m_rB, impulse);

	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

bool MouseJoint::SolvePositionConstraints(const SolverData& data)
{
	NOT_USED(data);
	return true;
}

glm::vec2 MouseJoint::GetAnchorA() const
{
	return m_targetA;
}

glm::vec2 MouseJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

glm::vec2 MouseJoint::GetReactionForce(real32 inv_dt) const
{
	return inv_dt * m_impulse;
}

real32 MouseJoint::GetReactionTorque(real32 inv_dt) const
{
	return inv_dt * 0.0f;
}

void MouseJoint::ShiftOrigin(const glm::vec2& newOrigin)
{
	m_targetA -= newOrigin;
}

