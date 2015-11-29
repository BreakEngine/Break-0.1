#pragma once
#include "Joint2D.hpp"

namespace Break
{
	namespace Physics
	{


		const  real32  _minPulleyLength = 2.0f;

		/// Pulley joint definition. This requires two ground anchors,
		/// two dynamic body anchor points, and a pulley ratio.
		struct BREAK_API PulleyJointDef : public JointDef
		{
			PulleyJointDef()
			{
				type = pulleyJoint;
				groundAnchorA = glm::vec2(-1.0f, 1.0f);
				groundAnchorB = glm::vec2(1.0f, 1.0f);
				localAnchorA = glm::vec2(-1.0f, 0.0f);
				localAnchorB = glm::vec2(1.0f, 0.0f);
				lengthA = 0.0f;
				lengthB = 0.0f;
				ratio = 1.0f;
				collideConnected = true;
			}

			/// Initialize the bodies, anchors, lengths, max lengths, and ratio using the world anchors.
			void Initialize(Body* bodyA, Body* bodyB,
				const glm::vec2& groundAnchorA, const glm::vec2& groundAnchorB,
				const glm::vec2& anchorA, const glm::vec2& anchorB,real32 ratio);

			/// The first ground anchor in world coordinates. This point never moves.
			glm::vec2 groundAnchorA;

			/// The second ground anchor in world coordinates. This point never moves.
			glm::vec2 groundAnchorB;

			/// The local anchor point relative to bodyA's origin.
			glm::vec2 localAnchorA;

			/// The local anchor point relative to bodyB's origin.
			glm::vec2 localAnchorB;

			/// The a reference length for the segment attached to bodyA.
			real32 lengthA;

			/// The a reference length for the segment attached to bodyB.
			real32 lengthB;

			/// The pulley ratio, used to simulate a block-and-tackle.
			real32 ratio;
		};

		/// The pulley joint is connected to two bodies and two fixed ground points.
		/// The pulley supports a ratio such that:
		/// length1 + ratio * length2 <= constant
		/// Yes, the force transmitted is scaled by the ratio.
		/// Warning: the pulley joint can get a bit squirrelly by itself. They often
		/// work better when combined with prismatic joints. You should also cover the
		/// the anchor points with static shapes to prevent one side from going to
		/// zero length.
		class BREAK_API PulleyJoint : public Joint
		{
		public:
			glm::vec2 GetAnchorA() const;
			glm::vec2 GetAnchorB() const;

			glm::vec2 GetReactionForce(real32 inv_dt) const;
			real32 GetReactionTorque(real32 inv_dt) const;

			/// Get the first ground anchor.
			glm::vec2 GetGroundAnchorA() const;

			/// Get the second ground anchor.
			glm::vec2 GetGroundAnchorB() const;

			/// Get the current length of the segment attached to bodyA.
			real32 GetLengthA() const;

			/// Get the current length of the segment attached to bodyB.
			real32 GetLengthB() const;

			/// Get the pulley ratio.
			real32 GetRatio() const;

			/// Get the current length of the segment attached to bodyA.
			real32 GetCurrentLengthA() const;

			/// Get the current length of the segment attached to bodyB.
			real32 GetCurrentLengthB() const;

			/// Dump joint to dmLog
			void Dump();

			/// Implement Joint::ShiftOrigin
			void ShiftOrigin(const glm::vec2& newOrigin);

		protected:

			friend class Joint;
			PulleyJoint(const PulleyJointDef* data);

			void InitVelocityConstraints(const SolverData& data);
			void SolveVelocityConstraints(const SolverData& data);
			bool SolvePositionConstraints(const SolverData& data);

			glm::vec2 m_groundAnchorA;
			glm::vec2 m_groundAnchorB;
			real32 m_lengthA;
			real32 m_lengthB;

			// Solver shared
			glm::vec2 m_localAnchorA;
			glm::vec2 m_localAnchorB;
			real32 m_constant;
			real32 m_ratio;
			real32 m_impulse;

			// Solver temp
			s32 m_indexA;
			s32 m_indexB;
			glm::vec2 m_uA;
			glm::vec2 m_uB;
			glm::vec2 m_rA;
			glm::vec2 m_rB;
			glm::vec2 m_localCenterA;
			glm::vec2 m_localCenterB;
			real32 m_invMassA;
			real32 m_invMassB;
			real32 m_invIA;
			real32 m_invIB;
			real32 m_mass;
		};


	}
}