#pragma once
#include "Joint2D.hpp"

namespace Break
{
	namespace Physics
	{

		/// Wheel joint definition. This requires defining a line of
		/// motion using an axis and an anchor point. The definition uses local
		/// anchor points and a local axis so that the initial configuration
		/// can violate the constraint slightly. The joint translation is zero
		/// when the local anchor points coincide in world space. Using local
		/// anchors and a local axis helps when saving and loading a game.
		struct BREAK_API WheelJointDef : public JointDef
		{
			WheelJointDef()
			{
				type = wheelJoint;
				localAnchorA = glm::vec2(0,0);
				localAnchorB = glm::vec2(0,0);
				localAxisA = glm::vec2(1.0f, 0.0f);
				enableMotor = false;
				maxMotorTorque = 0.0f;
				motorSpeed = 0.0f;
				frequencyHz = 2.0f;
				dampingRatio = 0.7f;
			}

			/// Initialize the bodies, anchors, axis, and reference angle using the world
			/// anchor and world axis.
			void Initialize(Body* bodyA, Body* bodyB, const glm::vec2& anchor, const glm::vec2& axis);

			/// The local anchor point relative to bodyA's origin.
			glm::vec2 localAnchorA;

			/// The local anchor point relative to bodyB's origin.
			glm::vec2 localAnchorB;

			/// The local translation axis in bodyA.
			glm::vec2 localAxisA;

			/// Enable/disable the joint motor.
			bool enableMotor;

			/// The maximum motor torque, usually in N-m.
			real32 maxMotorTorque;

			/// The desired motor speed in radians per second.
			real32 motorSpeed;

			/// Suspension frequency, zero indicates no suspension
			real32 frequencyHz;

			/// Suspension damping ratio, one indicates critical damping
			real32 dampingRatio;
		};

		/// A wheel joint. This joint provides two degrees of freedom: translation
		/// along an axis fixed in bodyA and rotation in the plane. You can use a
		/// joint limit to restrict the range of motion and a joint motor to drive
		/// the rotation or to model rotational friction.
		/// This joint is designed for vehicle suspensions.
		class BREAK_API WheelJoint : public Joint
		{
		public:
			glm::vec2 GetAnchorA() const;
			glm::vec2 GetAnchorB() const;

			glm::vec2 GetReactionForce(real32 inv_dt) const;
			real32 GetReactionTorque(real32 inv_dt) const;

			/// The local anchor point relative to bodyA's origin.
			const glm::vec2& GetLocalAnchorA() const { return m_localAnchorA; }

			/// The local anchor point relative to bodyB's origin.
			const glm::vec2& GetLocalAnchorB() const  { return m_localAnchorB; }

			/// The local joint axis relative to bodyA.
			const glm::vec2& GetLocalAxisA() const { return m_localXAxisA; }

			/// Get the current joint translation, usually in meters.
			real32 GetJointTranslation() const;

			/// Get the current joint translation speed, usually in meters per second.
			real32 GetJointSpeed() const;

			/// Is the joint motor enabled?
			bool IsMotorEnabled() const;

			/// Enable/disable the joint motor.
			void EnableMotor(bool flag);

			/// Set the motor speed, usually in radians per second.
			void SetMotorSpeed(real32 speed);

			/// Get the motor speed, usually in radians per second.
			real32 GetMotorSpeed() const;

			/// Set/Get the maximum motor force, usually in N-m.
			void SetMaxMotorTorque(real32 torque);
			real32 GetMaxMotorTorque() const;

			/// Get the current motor torque given the inverse time step, usually in N-m.
			real32 GetMotorTorque(real32 inv_dt) const;

			/// Set/Get the spring frequency in hertz. Setting the frequency to zero disables the spring.
			void SetSpringFrequencyHz(real32 hz);
			real32 GetSpringFrequencyHz() const;

			/// Set/Get the spring damping ratio
			void SetSpringDampingRatio(real32 ratio);
			real32 GetSpringDampingRatio() const;

			/// Dump to Log
			void Dump();

		protected:

			friend class Joint;
			WheelJoint(const WheelJointDef* def);

			void InitVelocityConstraints(const SolverData& data);
			void SolveVelocityConstraints(const SolverData& data);
			bool SolvePositionConstraints(const SolverData& data);

			real32 m_frequencyHz;
			real32 m_dampingRatio;

			// Solver shared
			glm::vec2 m_localAnchorA;
			glm::vec2 m_localAnchorB;
			glm::vec2 m_localXAxisA;
			glm::vec2 m_localYAxisA;

			real32 m_impulse;
			real32 m_motorImpulse;
			real32 m_springImpulse;

			real32 m_maxMotorTorque;
			real32 m_motorSpeed;
			bool m_enableMotor;

			// Solver temp
			s32 m_indexA;
			s32 m_indexB;
			glm::vec2 m_localCenterA;
			glm::vec2 m_localCenterB;
			real32 m_invMassA;
			real32 m_invMassB;
			real32 m_invIA;
			real32 m_invIB;

			glm::vec2 m_ax, m_ay;
			real32 m_sAx, m_sBx;
			real32 m_sAy, m_sBy;

			real32 m_mass;
			real32 m_motorMass;
			real32 m_springMass;

			real32 m_bias;
			real32 m_gamma;
		};

		inline real32 WheelJoint::GetMotorSpeed() const
		{
			return m_motorSpeed;
		}

		inline real32 WheelJoint::GetMaxMotorTorque() const
		{
			return m_maxMotorTorque;
		}

		inline void WheelJoint::SetSpringFrequencyHz(real32 hz)
		{
			m_frequencyHz = hz;
		}

		inline real32 WheelJoint::GetSpringFrequencyHz() const
		{
			return m_frequencyHz;
		}

		inline void WheelJoint::SetSpringDampingRatio(real32 ratio)
		{
			m_dampingRatio = ratio;
		}

		inline real32 WheelJoint::GetSpringDampingRatio() const
		{
			return m_dampingRatio;
		}


	}
}