#pragma once
#include "Joint2D.hpp"

namespace Break
{
	namespace Physics
	{

		/// Weld joint definition. You need to specify local anchor points
		/// where they are attached and the relative body angle. The position
		/// of the anchor points is important for computing the reaction torque.
		struct BREAK_API WeldJointDef : public JointDef
		{
			WeldJointDef()
			{
				type = weldJoint;
				localAnchorA = glm::vec2(0.0f, 0.0f);
				localAnchorB = glm::vec2(0.0f, 0.0f);
				referenceAngle = 0.0f;
				frequencyHz = 0.0f;
				dampingRatio = 0.0f;
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

			/// The mass-spring-damper frequency in Hertz. Rotation only.
			/// Disable softness with a value of 0.
			real32 frequencyHz;

			/// The damping ratio. 0 = no damping, 1 = critical damping.
			real32 dampingRatio;
		};

		/// A weld joint essentially glues two bodies together. A weld joint may
		/// distort somewhat because the island constraint solver is approximate.
		class BREAK_API WeldJoint : public Joint
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

			/// Get the reference angle.
			real32 GetReferenceAngle() const { return m_referenceAngle; }

			/// Set/get frequency in Hz.
			void SetFrequency(real32 hz) { m_frequencyHz = hz; }
			real32 GetFrequency() const { return m_frequencyHz; }

			/// Set/get damping ratio.
			void SetDampingRatio(real32 ratio) { m_dampingRatio = ratio; }
			real32 GetDampingRatio() const { return m_dampingRatio; }

			/// Dump to Log
			void Dump();

		protected:

			friend class Joint;

			WeldJoint(const WeldJointDef* def);

			void InitVelocityConstraints(const SolverData& data);
			void SolveVelocityConstraints(const SolverData& data);
			bool SolvePositionConstraints(const SolverData& data);

			real32 m_frequencyHz;
			real32 m_dampingRatio;
			real32 m_bias;

			// Solver shared
			glm::vec2 m_localAnchorA;
			glm::vec2 m_localAnchorB;
			real32 m_referenceAngle;
			real32 m_gamma;
			glm::vec3 m_impulse;

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
			glm::mat3 m_mass;
		};



	}
}