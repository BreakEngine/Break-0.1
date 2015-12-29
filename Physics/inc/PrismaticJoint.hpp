#pragma once
#include "Joint2D.hpp"

namespace Break
{

	namespace Physics
	{

		/// Prismatic joint definition. This requires defining a line of
		/// motion using an axis and an anchor point. The definition uses local
		/// anchor points and a local axis so that the initial configuration
		/// can violate the constraint slightly. The joint translation is zero
		/// when the local anchor points coincide in world space. Using local
		/// anchors and a local axis helps when saving and loading a game.
		struct BREAK_API PrismaticJointDef : public JointDef
		{
			PrismaticJointDef()
			{
				type = prismaticJoint;
				localAnchorA = glm::vec2(0,0);
				localAnchorB = glm::vec2(0,0);
				localAxisA = glm::vec2(1.0f, 0.0f);
				referenceAngle = 0.0f;
				enableLimit = false;
				lowerTranslation = 0.0f;
				upperTranslation = 0.0f;
				enableMotor = false;
				maxMotorForce = 0.0f;
				motorSpeed = 0.0f;
			}

			/// Initialize the bodies, anchors, axis, and reference angle using the world
			/// anchor and unit world axis.
			void Initialize(Body* bodyA, Body* bodyB, const glm::vec2& anchor, const glm::vec2& axis);

			/// The local anchor point relative to bodyA's origin.
			glm::vec2 localAnchorA;

			/// The local anchor point relative to bodyB's origin.
			glm::vec2 localAnchorB;

			/// The local translation unit axis in bodyA.
			glm::vec2 localAxisA;

			/// The constrained angle between the bodies: bodyB_angle - bodyA_angle.
			real32 referenceAngle;

			/// Enable/disable the joint limit.
			bool enableLimit;

			/// The lower translation limit, usually in meters.
			real32 lowerTranslation;

			/// The upper translation limit, usually in meters.
			real32 upperTranslation;

			/// Enable/disable the joint motor.
			bool enableMotor;

			/// The maximum motor torque, usually in N-m.
			real32 maxMotorForce;

			/// The desired motor speed in radians per second.
			real32 motorSpeed;
		};

		/// A prismatic joint. This joint provides one degree of freedom: translation
		/// along an axis fixed in bodyA. Relative rotation is prevented. You can
		/// use a joint limit to restrict the range of motion and a joint motor to
		/// drive the motion or to model joint friction.
		class BREAK_API PrismaticJoint : public Joint
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

			/// Get the reference angle.
			real32 GetReferenceAngle() const { return m_referenceAngle; }

			/// Get the current joint translation, usually in meters.
			real32 GetJointTranslation() const;

			/// Get the current joint translation speed, usually in meters per second.
			real32 GetJointSpeed() const;

			/// Is the joint limit enabled?
			bool IsLimitEnabled() const;

			/// Enable/disable the joint limit.
			void EnableLimit(bool flag);

			/// Get the lower joint limit, usually in meters.
			real32 GetLowerLimit() const;

			/// Get the upper joint limit, usually in meters.
			real32 GetUpperLimit() const;

			/// Set the joint limits, usually in meters.
			void SetLimits(real32 lower, real32 upper);

			/// Is the joint motor enabled?
			bool IsMotorEnabled() const;

			/// Enable/disable the joint motor.
			void EnableMotor(bool flag);

			/// Set the motor speed, usually in meters per second.
			void SetMotorSpeed(real32 speed);

			/// Get the motor speed, usually in meters per second.
			real32 GetMotorSpeed() const;

			/// Set the maximum motor force, usually in N.
			void SetMaxMotorForce(real32 force);
			real32 GetMaxMotorForce() const { return m_maxMotorForce; }

			/// Get the current motor force given the inverse time step, usually in N.
			real32 GetMotorForce(real32 inv_dt) const;

			/// Dump to Log
			void Dump();

		protected:
			friend class Joint;
			friend class GearJoint;
			PrismaticJoint(const PrismaticJointDef* def);

			void InitVelocityConstraints(const SolverData& data);
			void SolveVelocityConstraints(const SolverData& data);
			bool SolvePositionConstraints(const SolverData& data);

			// Solver shared
			glm::vec2 m_localAnchorA;
			glm::vec2 m_localAnchorB;
			glm::vec2 m_localXAxisA;
			glm::vec2 m_localYAxisA;
			real32 m_referenceAngle;
			glm::vec3 m_impulse;
			real32 m_motorImpulse;
			real32 m_lowerTranslation;
			real32 m_upperTranslation;
			real32 m_maxMotorForce;
			real32 m_motorSpeed;
			bool m_enableLimit;
			bool m_enableMotor;
			LimitState m_limitState;

			// Solver temp
			s32 m_indexA;
			s32 m_indexB;
			glm::vec2 m_localCenterA;
			glm::vec2 m_localCenterB;
			real32 m_invMassA;
			real32 m_invMassB;
			real32 m_invIA;
			real32 m_invIB;
			glm::vec2 m_axis, m_perp;
			real32 m_s1, m_s2;
			real32 m_a1, m_a2;
			glm::mat3 m_K;
			real32 m_motorMass;
		};

		inline real32 PrismaticJoint::GetMotorSpeed() const
		{
			return m_motorSpeed;
		}



	}

}