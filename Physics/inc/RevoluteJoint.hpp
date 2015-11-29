#pragma once
#include "Joint2D.hpp"

namespace Break
{
	namespace Physics
	{

		/// Revolute joint definition. This requires defining an
		/// anchor point where the bodies are joined. The definition
		/// uses local anchor points so that the initial configuration
		/// can violate the constraint slightly. You also need to
		/// specify the initial relative angle for joint limits. This
		/// helps when saving and loading a game.
		/// The local anchor points are measured from the body's origin
		/// rather than the center of mass because:
		/// 1. you might not know where the center of mass will be.
		/// 2. if you add/remove shapes from a body and recompute the mass,
		///    the joints will be broken.
		struct BREAK_API RevoluteJointDef : public JointDef
		{
			RevoluteJointDef()
			{
				type = revoluteJoint;
				localAnchorA = glm::vec2(0,0);
				localAnchorB = glm::vec2(0,0);
				referenceAngle = 0.0f;
				lowerAngle = 0.0f;
				upperAngle = 0.0f;
				maxMotorTorque = 0.0f;
				motorSpeed = 0.0f;
				enableLimit = false;
				enableMotor = false;
			}

			/// Initialize the bodies, anchors, and reference angle using a world
			/// anchor point.
			void Initialize(Body* bodyA, Body* bodyB, const glm::vec2& anchor);

			/// The local anchor point relative to bodyA's origin.
			glm::vec2 localAnchorA;

			/// The local anchor point relative to bodyB's origin.
			glm::vec2 localAnchorB;

			/// The bodyB angle minus bodyA angle in the reference state (radians).
			real32 referenceAngle;

			/// A flag to enable joint limits.
			bool enableLimit;

			/// The lower angle for the joint limit (radians).
			real32 lowerAngle;

			/// The upper angle for the joint limit (radians).
			real32 upperAngle;

			/// A flag to enable the joint motor.
			bool enableMotor;

			/// The desired motor speed. Usually in radians per second.
			real32 motorSpeed;

			/// The maximum motor torque used to achieve the desired motor speed.
			/// Usually in N-m.
			real32 maxMotorTorque;
		};

		/// A revolute joint constrains two bodies to share a common point while they
		/// are free to rotate about the point. The relative rotation about the shared
		/// point is the joint angle. You can limit the relative rotation with
		/// a joint limit that specifies a lower and upper angle. You can use a motor
		/// to drive the relative rotation about the shared point. A maximum motor torque
		/// is provided so that infinite forces are not generated.
		class BREAK_API RevoluteJoint : public Joint
		{
		public:
			glm::vec2 GetAnchorA() const;
			glm::vec2 GetAnchorB() const;

			/// The local anchor point relative to bodyA's origin.
			const glm::vec2& GetLocalAnchorA() const { return m_localAnchorA; }

			/// The local anchor point relative to bodyB's origin.
			const glm::vec2& GetLocalAnchorB() const  { return m_localAnchorB; }

			/// Get the reference angle.
			real32 GetReferenceAngle() const { return m_referenceAngle; }

			/// Get the current joint angle in radians.
			real32 GetJointAngle() const;

			/// Get the current joint angle speed in radians per second.
			real32 GetJointSpeed() const;

			/// Is the joint limit enabled?
			bool IsLimitEnabled() const;

			/// Enable/disable the joint limit.
			void EnableLimit(bool flag);

			/// Get the lower joint limit in radians.
			real32 GetLowerLimit() const;

			/// Get the upper joint limit in radians.
			real32 GetUpperLimit() const;

			/// Set the joint limits in radians.
			void SetLimits(real32 lower, real32 upper);

			/// Is the joint motor enabled?
			bool IsMotorEnabled() const;

			/// Enable/disable the joint motor.
			void EnableMotor(bool flag);

			/// Set the motor speed in radians per second.
			void SetMotorSpeed(real32 speed);

			/// Get the motor speed in radians per second.
			real32 GetMotorSpeed() const;

			/// Set the maximum motor torque, usually in N-m.
			void SetMaxMotorTorque(real32 torque);
			real32 GetMaxMotorTorque() const { return m_maxMotorTorque; }

			/// Get the reaction force given the inverse time step.
			/// Unit is N.
			glm::vec2 GetReactionForce(real32 inv_dt) const;

			/// Get the reaction torque due to the joint limit given the inverse time step.
			/// Unit is N*m.
			real32 GetReactionTorque(real32 inv_dt) const;

			/// Get the current motor torque given the inverse time step.
			/// Unit is N*m.
			real32 GetMotorTorque(real32 inv_dt) const;

			/// Dump to Log.
			void Dump();

		protected:

			friend class Joint;
			friend class GearJoint;

			RevoluteJoint(const RevoluteJointDef* def);

			void InitVelocityConstraints(const SolverData& data);
			void SolveVelocityConstraints(const SolverData& data);
			bool SolvePositionConstraints(const SolverData& data);

			// Solver shared
			glm::vec2 m_localAnchorA;
			glm::vec2 m_localAnchorB;
			glm::vec3 m_impulse;
			real32 m_motorImpulse;

			bool m_enableMotor;
			real32 m_maxMotorTorque;
			real32 m_motorSpeed;

			bool m_enableLimit;
			real32 m_referenceAngle;
			real32 m_lowerAngle;
			real32 m_upperAngle;

			// Solver temp
			s32 m_indexA;
			s32 m_indexB;
			glm::vec2 m_rA;
			glm::vec2 m_rB;
			glm::vec2 m_localCenterA;
			glm::vec2 m_localCenterB;
			real32 m_invMassA;
			real32 m_invMassB;
			real32 m_invIA;
			real32 m_invIB;
			glm::mat3 m_mass;			// effective mass for point-to-point constraint.
			real32 m_motorMass;	// effective mass for motor/limit angular constraint.
			LimitState m_limitState;
		};

		inline real32 RevoluteJoint::GetMotorSpeed() const
		{
			return m_motorSpeed;
		}




	}
}