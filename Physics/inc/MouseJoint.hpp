#pragma once
#include "Joint2D.hpp"

namespace Break
{

	namespace Physics
	{

		/// Mouse joint definition. This requires a world target point,
		/// tuning parameters, and the time step.
		struct BREAK_API MouseJointDef : public JointDef
		{
			MouseJointDef()
			{
				type = mouseJoint;
				target = glm::vec2(0.0f, 0.0f);
				maxForce = 0.0f;
				frequencyHz = 5.0f;
				dampingRatio = 0.7f;
			}

			/// The initial world target point. This is assumed
			/// to coincide with the body anchor initially.
			glm::vec2 target;

			/// The maximum constraint force that can be exerted
			/// to move the candidate body. Usually you will express
			/// as some multiple of the weight (multiplier * mass * gravity).
			real32 maxForce;

			/// The response speed.
			real32 frequencyHz;

			/// The damping ratio. 0 = no damping, 1 = critical damping.
			real32 dampingRatio;
		};

		/// A mouse joint is used to make a point on a body track a
		/// specified world point. This a soft constraint with a maximum
		/// force. This allows the constraint to stretch and without
		/// applying huge forces.
		/// NOTE: this joint is not documented in the manual because it was
		/// developed to be used in the testbed. If you want to learn how to
		/// use the mouse joint, look at the testbed.
		class BREAK_API MouseJoint : public Joint
		{
		public:

			/// Implements Joint.
			glm::vec2 GetAnchorA() const;

			/// Implements Joint.
			glm::vec2 GetAnchorB() const;

			/// Implements Joint.
			glm::vec2 GetReactionForce(real32 inv_dt) const;

			/// Implements Joint.
			real32 GetReactionTorque(real32 inv_dt) const;

			/// Use this to update the target point.
			void SetTarget(const glm::vec2& target);
			const glm::vec2& GetTarget() const;

			/// Set/get the maximum force in Newtons.
			void SetMaxForce(real32 force);
			real32 GetMaxForce() const;

			/// Set/get the frequency in Hertz.
			void SetFrequency(real32 hz);
			real32 GetFrequency() const;

			/// Set/get the damping ratio (dimensionless).
			void SetDampingRatio(real32 ratio);
			real32 GetDampingRatio() const;

			/// The mouse joint does not support dumping.
			void Dump() { printf("Mouse joint dumping is not supported.\n"); }

			/// Implement Joint::ShiftOrigin
			void ShiftOrigin(const glm::vec2& newOrigin);

		protected:
			friend class Joint;

			MouseJoint(const MouseJointDef* def);

			void InitVelocityConstraints(const SolverData& data);
			void SolveVelocityConstraints(const SolverData& data);
			bool SolvePositionConstraints(const SolverData& data);

			glm::vec2 m_localAnchorB;
			glm::vec2 m_targetA;
			real32 m_frequencyHz;
			real32 m_dampingRatio;
			real32 m_beta;

			// Solver shared
			glm::vec2 m_impulse;
			real32 m_maxForce;
			real32 m_gamma;

			// Solver temp
			s32 m_indexA;
			s32 m_indexB;
			glm::vec2 m_rB;
			glm::vec2 m_localCenterB;
			real32 m_invMassB;
			real32 m_invIB;
			glm::mat2 m_mass;
			glm::vec2 m_C;
		};


	}

}
