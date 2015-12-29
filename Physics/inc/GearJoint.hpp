#pragma once
#include "Joint2D.hpp"

namespace Break
{
	namespace Physics
	{

		/// Gear joint definition. This definition requires two existing
		/// revolute or prismatic joints (any combination will work).
		struct BREAK_API GearJointDef : public JointDef
		{
			GearJointDef()
			{
				type = gearJoint;
				joint1 = NULL;
				joint2 = NULL;
				ratio = 1.0f;
			}

			/// The first revolute/prismatic joint attached to the gear joint.
			Joint* joint1;

			/// The second revolute/prismatic joint attached to the gear joint.
			Joint* joint2;

			/// The gear ratio.
			/// @see GearJoint for explanation.
			real32 ratio;
		};

		/// A gear joint is used to connect two joints together. Either joint
		/// can be a revolute or prismatic joint. You specify a gear ratio
		/// to bind the motions together:
		/// coordinate1 + ratio * coordinate2 = constant
		/// The ratio can be negative or positive. If one joint is a revolute joint
		/// and the other joint is a prismatic joint, then the ratio will have units
		/// of length or units of 1/length.
		/// @warning You have to manually destroy the gear joint if joint1 or joint2
		/// is destroyed.
		class BREAK_API GearJoint : public Joint
		{
		public:
			glm::vec2 GetAnchorA() const;
			glm::vec2 GetAnchorB() const;

			glm::vec2 GetReactionForce(real32 inv_dt) const;
			real32 GetReactionTorque(real32 inv_dt) const;

			/// Get the first joint.
			Joint* GetJoint1() { return m_joint1; }

			/// Get the second joint.
			Joint* GetJoint2() { return m_joint2; }

			/// Set/Get the gear ratio.
			void SetRatio(real32 ratio);
			real32 GetRatio() const;

			/// Dump joint to dmLog
			void Dump();

		protected:

			friend class Joint;
			GearJoint(const GearJointDef* data);

			void InitVelocityConstraints(const SolverData& data);
			void SolveVelocityConstraints(const SolverData& data);
			bool SolvePositionConstraints(const SolverData& data);

			Joint* m_joint1;
			Joint* m_joint2;

			JointType m_typeA;
			JointType m_typeB;

			// Body A is connected to body C
			// Body B is connected to body D
			Body* m_bodyC;
			Body* m_bodyD;

			// Solver shared
			glm::vec2 m_localAnchorA;
			glm::vec2 m_localAnchorB;
			glm::vec2 m_localAnchorC;
			glm::vec2 m_localAnchorD;

			glm::vec2 m_localAxisC;
			glm::vec2 m_localAxisD;

			real32 m_referenceAngleA;
			real32 m_referenceAngleB;

			real32 m_constant;
			real32 m_ratio;

			real32 m_impulse;

			// Solver temp
			s32 m_indexA, m_indexB, m_indexC, m_indexD;
			glm::vec2 m_lcA, m_lcB, m_lcC, m_lcD;
			real32 m_mA, m_mB, m_mC, m_mD;
			real32 m_iA, m_iB, m_iC, m_iD;
			glm::vec2 m_JvAC, m_JvBD;
			real32 m_JwA, m_JwB, m_JwC, m_JwD;
			real32 m_mass;
		};





	}
}