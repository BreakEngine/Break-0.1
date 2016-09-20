#pragma once
#include "MathUtils.hpp"
#include "Collision.hpp"
#include "TimeStep.hpp"
#include "PTimeStep.hpp"
#include "PhysicsGlobals.hpp"
#include "Profile.hpp"

namespace Break
{
	namespace Physics
	{


		class BREAK_API Contact;
		class BREAK_API Body;
		class BREAK_API StackAllocator;
		struct BREAK_API ContactPositionConstraint;

		struct BREAK_API VelocityConstraintPoint
		{
			glm::vec2 rA;
			glm::vec2 rB;
			real32 normalImpulse;
			real32 tangentImpulse;
			real32 normalMass;
			real32 tangentMass;
			real32 velocityBias;
		};

		struct BREAK_API ContactVelocityConstraint
		{
			VelocityConstraintPoint points[maxManifoldPoints];
			glm::vec2 normal;
			glm::mat2 normalMass;
			glm::mat2 K; 
			s32 indexA;
			s32 indexB;
			real32 invMassA, invMassB;
			real32 invIA, invIB;
			real32 friction;
			real32 restitution;
			real32 tangentSpeed;
			s32 pointCount;
			s32 contactIndex;
		};

		struct BREAK_API ContactSolverDef
		{
			PTimeStep step;
			Contact** contacts;
			s32 count;
			Position* positions;
			Velocity* velocities;
			StackAllocator* allocator;
		};

		class BREAK_API ContactSolver
		{
		public:
			ContactSolver(ContactSolverDef* def);
			~ContactSolver();

			void InitializeVelocityConstraints();

			void WarmStart();
			void SolveVelocityConstraints();
			void StoreImpulses();

			bool SolvePositionConstraints();
			bool SolveTOIPositionConstraints(s32 toiIndexA, s32 toiIndexB);

			PTimeStep m_step;
			Position* m_positions;
			Velocity* m_velocities;
			StackAllocator* m_allocator;
			ContactPositionConstraint* m_positionConstraints;
			ContactVelocityConstraint* m_velocityConstraints;
			Contact** m_contacts;
			int m_count;
		};




	}

}