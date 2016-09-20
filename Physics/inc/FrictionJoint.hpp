#pragma once
#include "Joint2D.hpp"

namespace Break
{
	namespace Physics
	{

		/// Friction joint definition.
		struct BREAK_API FrictionJointDef : public JointDef
		{
			FrictionJointDef()
			{
				type = frictionJoint;
				localAnchorA = glm::vec2(0,0);
				localAnchorB = glm::vec2(0,0);
				maxForce = 0.0f;
				maxTorque = 0.0f;
			}

			/// Initialize the bodies, anchors, axis, and reference angle using the world
			/// anchor and world axis.
			void Initialize(Body* bodyA, Body* bodyB, const glm::vec2& anchor);

			/// The local anchor point relative to bodyA's origin.
			glm::vec2 localAnchorA;

			/// The local anchor point relative to bodyB's origin.
			glm::vec2 localAnchorB;

			/// The maximum friction force in N.
			real32 maxForce;

			/// The maximum friction torque in N-m.
			real32 maxTorque;
		};

		/// Friction joint. This is used for top-down friction.
		/// It provides 2D translational friction and angular friction.
		class BREAK_API FrictionJoint : public Joint
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

			/// Set the maximum friction force in N.
			void SetMaxForce(real32 force);

			/// Get the maximum friction force in N.
			real32 GetMaxForce() const;

			/// Set the maximum friction torque in N*m.
			void SetMaxTorque(real32 torque);

			/// Get the maximum friction torque in N*m.
			real32 GetMaxTorque() const;

			/// Dump joint to dmLog
			void Dump();

		protected:

			friend class Joint;

			FrictionJoint(const FrictionJointDef* def);

			void InitVelocityConstraints(const SolverData& data);
			void SolveVelocityConstraints(const SolverData& data);
			bool SolvePositionConstraints(const SolverData& data);

			glm::vec2 m_localAnchorA;
			glm::vec2 m_localAnchorB;

			// Solver shared
			glm::vec2 m_linearImpulse;
			real32 m_angularImpulse;
			real32 m_maxForce;
			real32 m_maxTorque;

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
			glm::mat2 m_linearMass;
			real32 m_angularMass;
		};



	}
}