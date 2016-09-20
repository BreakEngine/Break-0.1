#pragma once
#include "Joint2D.hpp"

namespace Break
{
	namespace Physics
	{

		/// Distance joint definition. This requires defining an
		/// anchor point on both bodies and the non-zero length of the
		/// distance joint. The definition uses local anchor points
		/// so that the initial configuration can violate the constraint
		/// slightly. This helps when saving and loading a game.
		/// @warning Do not use a zero or short length.
		struct BREAK_API DistanceJointDef : public JointDef
		{
			DistanceJointDef()
			{
				type = distanceJoint;
				localAnchorA = glm::vec2(0.0f, 0.0f);
				localAnchorB = glm::vec2(0.0f, 0.0f);
				length = 1.0f;
				frequencyHz = 0.0f;
				dampingRatio = 0.0f;
			}

			/// Initialize the bodies, anchors, and length using the world
			/// anchors.
			void Initialize(Body* bodyA, Body* bodyB,const glm::vec2& anchorA, const glm::vec2& anchorB);

			/// The local anchor point relative to bodyA's origin.
			glm::vec2 localAnchorA;

			/// The local anchor point relative to bodyB's origin.
			glm::vec2 localAnchorB;

			/// The natural length between the anchor points.
			real32 length;

			/// The mass-spring-damper frequency in Hertz. A value of 0
			/// disables softness.
			real32 frequencyHz;

			/// The damping ratio. 0 = no damping, 1 = critical damping.
			real32 dampingRatio;
		};

		/// A distance joint constrains two points on two bodies
		/// to remain at a fixed distance from each other. You can view
		/// this as a massless, rigid rod.
		class BREAK_API DistanceJoint : public Joint
		{
		public:

			glm::vec2 GetAnchorA() const;
			glm::vec2 GetAnchorB() const;

			/// Get the reaction force given the inverse time step.
			/// Unit is N.
			glm::vec2 GetReactionForce(real32 inv_dt) const;

			/// Get the reaction torque given the inverse time step.
			/// Unit is N*m. This is always zero for a distance joint.
			real32 GetReactionTorque(real32 inv_dt) const;

			/// The local anchor point relative to bodyA's origin.
			const glm::vec2& GetLocalAnchorA() const { return m_localAnchorA; }

			/// The local anchor point relative to bodyB's origin.
			const glm::vec2& GetLocalAnchorB() const  { return m_localAnchorB; }

			/// Set/get the natural length.
			/// Manipulating the length can lead to non-physical behavior when the frequency is zero.
			void SetLength(real32 length);
			real32 GetLength() const;

			/// Set/get frequency in Hz.
			void SetFrequency(real32 hz);
			real32 GetFrequency() const;

			/// Set/get damping ratio.
			void SetDampingRatio(real32 ratio);
			real32 GetDampingRatio() const;

			/// Dump joint to dmLog
			void Dump();

		protected:

			friend class Joint;
			DistanceJoint(const DistanceJointDef* data);

			void InitVelocityConstraints(const SolverData& data);
			void SolveVelocityConstraints(const SolverData& data);
			bool SolvePositionConstraints(const SolverData& data);

			real32 m_frequencyHz;
			real32 m_dampingRatio;
			real32 m_bias;

			// Solver shared
			glm::vec2 m_localAnchorA;
			glm::vec2 m_localAnchorB;
			real32 m_gamma;
			real32 m_impulse;
			real32 m_length;

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
		};

		inline void DistanceJoint::SetLength(real32 length)
		{
			m_length = length;
		}

		inline real32 DistanceJoint::GetLength() const
		{
			return m_length;
		}

		inline void DistanceJoint::SetFrequency(real32 hz)
		{
			m_frequencyHz = hz;
		}

		inline real32 DistanceJoint::GetFrequency() const
		{
			return m_frequencyHz;
		}

		inline void DistanceJoint::SetDampingRatio(real32 ratio)
		{
			m_dampingRatio = ratio;
		}

		inline real32 DistanceJoint::GetDampingRatio() const
		{
			return m_dampingRatio;
		}


	}
}