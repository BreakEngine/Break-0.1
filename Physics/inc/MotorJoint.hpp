#pragma once
#include "Joint2D.hpp"

namespace Break
{

	namespace Physics
	{

		/// Motor joint definition.
		struct BREAK_API MotorJointDef : public JointDef
		{
			MotorJointDef()
			{
				type = motorJoint;
				linearOffset = glm::vec2(0,0);
				angularOffset = 0.0f;
				maxForce = 1.0f;
				maxTorque = 1.0f;
				correctionFactor = 0.3f;
			}

			/// Initialize the bodies and offsets using the current transforms.
			void Initialize(Body* bodyA, Body* bodyB);

			/// Position of bodyB minus the position of bodyA, in bodyA's frame, in meters.
			glm::vec2 linearOffset;

			/// The bodyB angle minus bodyA angle in radians.
			real32 angularOffset;

			/// The maximum motor force in N.
			real32 maxForce;

			/// The maximum motor torque in N-m.
			real32 maxTorque;

			/// Position correction factor in the range [0,1].
			real32 correctionFactor;
		};

		/// A motor joint is used to control the relative motion
		/// between two bodies. A typical usage is to control the movement
		/// of a dynamic body with respect to the ground.
		class BREAK_API MotorJoint : public Joint
		{
		public:
			glm::vec2 GetAnchorA() const;
			glm::vec2 GetAnchorB() const;

			glm::vec2 GetReactionForce(real32 inv_dt) const;
			real32 GetReactionTorque(real32 inv_dt) const;

			/// Set/get the target linear offset, in frame A, in meters.
			void SetLinearOffset(const glm::vec2& linearOffset);
			const glm::vec2& GetLinearOffset() const;

			/// Set/get the target angular offset, in radians.
			void SetAngularOffset(real32 angularOffset);
			real32 GetAngularOffset() const;

			/// Set the maximum friction force in N.
			void SetMaxForce(real32 force);

			/// Get the maximum friction force in N.
			real32 GetMaxForce() const;

			/// Set the maximum friction torque in N*m.
			void SetMaxTorque(real32 torque);

			/// Get the maximum friction torque in N*m.
			real32 GetMaxTorque() const;

			/// Set the position correction factor in the range [0,1].
			void SetCorrectionFactor(real32 factor);

			/// Get the position correction factor in the range [0,1].
			real32 GetCorrectionFactor() const;

			/// Dump to Log
			void Dump();

		protected:

			friend class Joint;

			MotorJoint(const MotorJointDef* def);

			void InitVelocityConstraints(const SolverData& data);
			void SolveVelocityConstraints(const SolverData& data);
			bool SolvePositionConstraints(const SolverData& data);

			// Solver shared
			glm::vec2 m_linearOffset;
			real32 m_angularOffset;
			glm::vec2 m_linearImpulse;
			real32 m_angularImpulse;
			real32 m_maxForce;
			real32 m_maxTorque;
			real32 m_correctionFactor;

			// Solver temp
			s32 m_indexA;
			s32 m_indexB;
			glm::vec2 m_rA;
			glm::vec2 m_rB;
			glm::vec2 m_localCenterA;
			glm::vec2 m_localCenterB;
			glm::vec2 m_linearError;
			real32 m_angularError;
			real32 m_invMassA;
			real32 m_invMassB;
			real32 m_invIA;
			real32 m_invIB;
			glm::mat2 m_linearMass;
			real32 m_angularMass;
		};




	}
}