#pragma once
#include "Joint2D.hpp"

namespace Break
{
	namespace Physics
	{

		/// Rope joint definition. This requires two body anchor points and
		/// a maximum lengths.
		/// Note: by default the connected objects will not collide.
		/// see collideConnected in JointDef.
		struct BREAK_API RopeJointDef : public JointDef
		{
			RopeJointDef()
			{
				type = ropeJoint;
				localAnchorA = glm::vec2(-1.0f, 0.0f);
				localAnchorB = glm::vec2(1.0f, 0.0f);
				maxLength = 0.0f;
			}

			/// The local anchor point relative to bodyA's origin.
			glm::vec2 localAnchorA;

			/// The local anchor point relative to bodyB's origin.
			glm::vec2 localAnchorB;

			/// The maximum length of the rope.
			/// Warning: this must be larger than _linearSlop or
			/// the joint will have no effect.
			real32 maxLength;
		};

		/// A rope joint enforces a maximum distance between two points
		/// on two bodies. It has no other effect.
		/// Warning: if you attempt to change the maximum length during
		/// the simulation you will get some non-physical behavior.
		/// A model that would allow you to dynamically modify the length
		/// would have some sponginess, so I chose not to implement it
		/// that way. See DistanceJoint if you want to dynamically
		/// control length.
		class BREAK_API RopeJoint : public Joint
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

			/// Set/Get the maximum length of the rope.
			void SetMaxLength(real32 length) { m_maxLength = length; }
			real32 GetMaxLength() const;

			LimitState GetLimitState() const;

			/// Dump joint to dmLog
			void Dump();

		protected:

			friend class Joint;
			RopeJoint(const RopeJointDef* data);

			void InitVelocityConstraints(const SolverData& data);
			void SolveVelocityConstraints(const SolverData& data);
			bool SolvePositionConstraints(const SolverData& data);

			// Solver shared
			glm::vec2 m_localAnchorA;
			glm::vec2 m_localAnchorB;
			real32 m_maxLength;
			real32 m_length;
			real32 m_impulse;

			// Solver temp
			s32 m_indexA;
			s32 m_indexB;
			glm::vec2 m_u;
			glm::vec2 m_rA;
			glm::vec2 m_rB;
			glm::vec2 m_localCenterA;
			glm::vec2 m_localCenterB;
			real32 m_invMassA;
			real32 m_invMassB;
			real32 m_invIA;
			real32 m_invIB;
			real32 m_mass;
			LimitState m_state;
		};





	}

}