#pragma once
#include "MathUtils.hpp"
#include "Body2D.hpp"
#include "TimeStep.hpp"
#include "Profile.hpp"

namespace Break
{

	namespace Physics
	{
		class BREAK_API PTimeStep;
		struct BREAK_API Profile;

		class BREAK_API Contact;
		class BREAK_API Joint;
		class BREAK_API StackAllocator;
		class BREAK_API ContactListener;
		struct BREAK_API ContactVelocityConstraint;
		///struct BREAK_API Profile;

		/// This is an internal class.
		class BREAK_API Island 
		{
		public:
			Island(s32 bodyCapacity, s32 contactCapacity, s32 jointCapacity,
				StackAllocator* allocator, ContactListener* listener);
			~Island();

			void Clear()
			{
				m_bodyCount = 0;
				m_contactCount = 0;
				m_jointCount = 0;
			}

			void Solve(Profile* profile, const PTimeStep& step, const glm::vec2& gravity, bool allowSleep);

			void SolveTOI(const PTimeStep& subStep, s32 toiIndexA, s32 toiIndexB);

			void Add(Body* body)
			{
				assert(m_bodyCount < m_bodyCapacity);
				body->m_islandIndex = m_bodyCount;
				m_bodies[m_bodyCount] = body;
				++m_bodyCount;
			}

			void Add(Contact* contact)
			{
				assert(m_contactCount < m_contactCapacity);
				m_contacts[m_contactCount++] = contact;
			}

			void Add(Joint* joint)
			{
				assert(m_jointCount < m_jointCapacity);
				m_joints[m_jointCount++] = joint;
			}

			void Report(const ContactVelocityConstraint* constraints);

			StackAllocator* m_allocator;
			ContactListener* m_listener;

			Body** m_bodies;
			Contact** m_contacts;
			Joint** m_joints;

			Position* m_positions;
			Velocity* m_velocities;

			s32 m_bodyCount;
			s32 m_jointCount;
			s32 m_contactCount;

			s32 m_bodyCapacity;
			s32 m_contactCapacity;
			s32 m_jointCapacity;
		};


	}

}
