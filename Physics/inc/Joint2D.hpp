#pragma once
#include "MathUtils.hpp"
#include "TimeStep.hpp"
#include "Profile.hpp"
#include "PhysicsGlobals.hpp"

namespace Break
{
	namespace Physics
	{

		class BREAK_API Body;
		class BREAK_API Joint;
		//struct BREAK_API SolverData;
		class BREAK_API BlockAllocator;

		enum BREAK_API JointType
		{
			unknownJoint,
			revoluteJoint,
			prismaticJoint,
			distanceJoint,
			pulleyJoint,
			mouseJoint,
			gearJoint,
			wheelJoint,
			weldJoint,
			frictionJoint,
			ropeJoint,
			motorJoint
		};

		enum BREAK_API LimitState
		{
			inactiveLimit,
			atLowerLimit,
			atUpperLimit,
			equalLimits
		};

		struct BREAK_API Jacobian
		{
			glm::vec2 linear;
			real32 angularA;
			real32 angularB;
		};

		/// A joint edge is used to connect bodies and joints together
		/// in a joint graph where each body is a node and each joint
		/// is an edge. A joint edge belongs to a doubly linked list
		/// maintained in each attached body. Each joint has two joint
		/// nodes, one for each attached body.
		struct BREAK_API JointEdge
		{
			Body* other;			///< provides quick access to the other body attached.
			Joint* joint;			///< the joint
			JointEdge* prev;		///< the previous joint edge in the body's joint list
			JointEdge* next;		///< the next joint edge in the body's joint list
		};

		/// Joint definitions are used to construct joints.
		struct BREAK_API JointDef
		{
			JointDef()
			{
				type = unknownJoint;
				userData = NULL;
				bodyA = NULL;
				bodyB = NULL;
				collideConnected = false;
			}

			/// The joint type is set automatically for concrete joint types.
			JointType type;

			/// Use this to attach application specific data to your joints.
			void* userData;

			/// The first attached body.
			Body* bodyA;

			/// The second attached body.
			Body* bodyB;

			/// Set this flag to true if the attached bodies should collide.
			bool collideConnected;
		};

		/// The base joint class. Joints are used to constraint two bodies together in
		/// various fashions. Some joints also feature limits and motors.
		class BREAK_API Joint
		{
		public:

			/// Get the type of the concrete joint.
			JointType GetType() const;

			/// Get the first body attached to this joint.
			Body* GetBodyA();

			/// Get the second body attached to this joint.
			Body* GetBodyB();

			/// Get the anchor point on bodyA in world coordinates.
			virtual glm::vec2 GetAnchorA() const = 0;

			/// Get the anchor point on bodyB in world coordinates.
			virtual glm::vec2 GetAnchorB() const = 0;

			/// Get the reaction force on bodyB at the joint anchor in Newtons.
			virtual glm::vec2 GetReactionForce(real32 inv_dt) const = 0;

			/// Get the reaction torque on bodyB in N*m.
			virtual real32 GetReactionTorque(real32 inv_dt) const = 0;

			/// Get the next joint the world joint list.
			Joint* GetNext();
			const Joint* GetNext() const;

			/// Get the user data pointer.
			void* GetUserData() const;

			/// Set the user data pointer.
			void SetUserData(void* data);

			/// Short-cut function to determine if either body is inactive.
			bool IsActive() const;

			/// Get collide connected.
			/// Note: modifying the collide connect flag won't work correctly because
			/// the flag is only checked when fixture AABBs begin to overlap.
			bool GetCollideConnected() const;

			/// Dump this joint to the log file.
			virtual void Dump() { printf("// Dump is not supported for this joint type.\n"); }

			/// Shift the origin for any points stored in world coordinates.
			virtual void ShiftOrigin(const glm::vec2& newOrigin) { NOT_USED(newOrigin);  }

		protected:
			friend class World;
			friend class Body;
			friend class Island;
			friend class GearJoint;

			static Joint* Create(const JointDef* def, BlockAllocator* allocator);
			static void Destroy(Joint* joint, BlockAllocator* allocator);

			Joint(const JointDef* def);
			virtual ~Joint() {}

			virtual void InitVelocityConstraints(const SolverData& data) = 0;
			virtual void SolveVelocityConstraints(const SolverData& data) = 0;

			// This returns true if the position errors are within tolerance.
			virtual bool SolvePositionConstraints(const SolverData& data) = 0;

			JointType m_type;
			Joint* m_prev;
			Joint* m_next;
			JointEdge m_edgeA;
			JointEdge m_edgeB;
			Body* m_bodyA;
			Body* m_bodyB;

			s32 m_index;

			bool m_islandFlag;
			bool m_collideConnected;

			void* m_userData;
		};

		inline JointType Joint::GetType() const
		{
			return m_type;
		}

		inline Body* Joint::GetBodyA()
		{
			return m_bodyA;
		}

		inline Body* Joint::GetBodyB()
		{
			return m_bodyB;
		}

		inline Joint* Joint::GetNext()
		{
			return m_next;
		}

		inline const Joint* Joint::GetNext() const
		{
			return m_next;
		}

		inline void* Joint::GetUserData() const
		{
			return m_userData;
		}

		inline void Joint::SetUserData(void* data)
		{
			m_userData = data;
		}

		inline bool Joint::GetCollideConnected() const
		{
			return m_collideConnected;
		}


	}
}