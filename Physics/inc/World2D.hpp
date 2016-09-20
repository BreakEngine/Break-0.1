#pragma once
#include "MathUtils.hpp"
#include "BlockAllocator.hpp"
#include "StackAllocator.hpp"
#include "ContactManager.hpp"
#include "WorldCallBacks.hpp"
#include "PTimeStep.hpp"
#include "Profile.hpp"
namespace Break
{

	namespace Physics
	{

		struct BREAK_API AABB;
		struct BREAK_API BodyDef;
		struct BREAK_API JointDef;
		class BREAK_API Body;
		class BREAK_API Fixture;
		class BREAK_API Joint;

		/// The world class manages all physics entities, dynamic simulation,
		/// and asynchronous queries. The world also contains efficient memory
		/// management facilities.
		class BREAK_API World
		{
		public:
			/// Construct a world object.
			/// @param gravity the world gravity vector.
			World(const glm::vec2& gravity); 

			/// Destruct the world. All physics entities are destroyed and all heap memory is released.
			~World();

			/// Register a destruction listener. The listener is owned by you and must
			/// remain in scope.
			void SetDestructionListener(DestructionListener* listener);

			/// Register a contact filter to provide specific control over collision.
			/// Otherwise the default filter is used (_defaultFilter). The listener is
			/// owned by you and must remain in scope. 
			void SetContactFilter(ContactFilter* filter);

			/// Register a contact event listener. The listener is owned by you and must
			/// remain in scope.
			void SetContactListener(ContactListener* listener);


			/// Create a rigid body given a definition. No reference to the definition
			/// is retained.
			/// @warning This function is locked during callbacks.
			Body* CreateBody(const BodyDef* def);

			/// Destroy a rigid body given a definition. No reference to the definition
			/// is retained. This function is locked during callbacks.
			/// @warning This automatically deletes all associated shapes and joints.
			/// @warning This function is locked during callbacks.
			void DestroyBody(Body* body);

			/// Create a joint to constrain bodies together. No reference to the definition
			/// is retained. This may cause the connected bodies to cease colliding.
			/// @warning This function is locked during callbacks.
			Joint* CreateJoint(const JointDef* def);

			/// Destroy a joint. This may cause the connected bodies to begin colliding.
			/// @warning This function is locked during callbacks.
			void DestroyJoint(Joint* joint);

			/// Take a time step. This performs collision detection, integration,
			/// and constraint solution.
			/// @param timeStep the amount of time to simulate, this should not vary.
			/// @param velocityIterations for the velocity constraint solver.
			/// @param positionIterations for the position constraint solver.
			void Step(real32 timeStep,s32 velocityIterations,s32 positionIterations);

			/// Manually clear the force buffer on all bodies. By default, forces are cleared automatically
			/// after each call to Step. The default behavior is modified by calling SetAutoClearForces.
			/// The purpose of this function is to support sub-stepping. Sub-stepping is often used to maintain
			/// a fixed sized time step under a variable frame-rate.
			/// When you perform sub-stepping you will disable auto clearing of forces and instead call
			/// ClearForces after all sub-steps are complete in one pass of your game loop.
			/// @see SetAutoClearForces
			void ClearForces();

			/// Call this to draw shapes and other debug draw data. This is intentionally non-const.
			void DrawDebugData();

			/// Query the world for all fixtures that potentially overlap the
			/// provided AABB.
			/// @param callback a user implemented callback class.
			/// @param aabb the query box.
			void QueryAABB(QueryCallback* callback, const AABB& aabb) const;

			/// Ray-cast the world for all fixtures in the path of the ray. Your callback
			/// controls whether you get the closest point, any point, or n-points.
			/// The ray-cast ignores shapes that contain the starting point.
			/// @param callback a user implemented callback class.
			/// @param point1 the ray starting point
			/// @param point2 the ray ending point
			void RayCast(RayCastCallback* callback, const glm::vec2& point1, const glm::vec2& point2) const;

			/// Get the world body list. With the returned body, use Body::GetNext to get
			/// the next body in the world list. A NULL body indicates the end of the list.
			/// @return the head of the world body list.
			Body* GetBodyList();
			const Body* GetBodyList() const;

			/// Get the world joint list. With the returned joint, use Joint::GetNext to get
			/// the next joint in the world list. A NULL joint indicates the end of the list.
			/// @return the head of the world joint list.
			Joint* GetJointList();
			const Joint* GetJointList() const;

			/// Get the world contact list. With the returned contact, use Contact::GetNext to get
			/// the next contact in the world list. A NULL contact indicates the end of the list.
			/// @return the head of the world contact list.
			/// @warning contacts are created and destroyed in the middle of a time step.
			/// Use ContactListener to avoid missing contacts.
			Contact* GetContactList();
			const Contact* GetContactList() const;

			/// Enable/disable sleep.
			void SetAllowSleeping(bool flag);
			bool GetAllowSleeping() const { return m_allowSleep; }

			/// Enable/disable warm starting. For testing.
			void SetWarmStarting(bool flag) { m_warmStarting = flag; }
			bool GetWarmStarting() const { return m_warmStarting; }

			/// Enable/disable continuous physics. For testing.
			void SetContinuousPhysics(bool flag) { m_continuousPhysics = flag; }
			bool GetContinuousPhysics() const { return m_continuousPhysics; }

			/// Enable/disable single stepped continuous physics. For testing.
			void SetSubStepping(bool flag) { m_subStepping = flag; }
			bool GetSubStepping() const { return m_subStepping; }

			/// Get the number of broad-phase proxies.
			s32 GetProxyCount() const;

			/// Get the number of bodies.
			s32 GetBodyCount() const;

			/// Get the number of joints.
			s32 GetJointCount() const;

			/// Get the number of contacts (each may have 0 or more contact points).
			s32 GetContactCount() const;

			/// Get the height of the dynamic tree.
			s32 GetTreeHeight() const;

			/// Get the balance of the dynamic tree.
			s32 GetTreeBalance() const;

			/// Get the quality metric of the dynamic tree. The smaller the better.
			/// The minimum is 1.
			real32 GetTreeQuality() const;

			/// Change the global gravity vector.
			void SetGravity(const glm::vec2& gravity);

			/// Get the global gravity vector.
			glm::vec2 GetGravity() const;

			/// Is the world locked (in the middle of a time step).
			bool IsLocked() const;

			/// Set flag to control automatic clearing of forces after each time step.
			void SetAutoClearForces(bool flag);

			/// Get the flag that controls automatic clearing of forces after each time step.
			bool GetAutoClearForces() const;

			/// Shift the world origin. Useful for large worlds.
			/// The body shift formula is: position -= newOrigin
			/// @param newOrigin the new origin with respect to the old origin
			void ShiftOrigin(const glm::vec2& newOrigin);

			/// Get the contact manager for testing.
			const ContactManager& GetContactManager() const;

			/// Get the current profile.
			const Profile& GetProfile() const;

			/// Dump the world into the log file.
			/// @warning this should be called outside of a time step.
			void Dump();

		private:

			// m_flags
			enum
			{
				newFixture	= 0x0001,
				locked		= 0x0002,
				clearForces	= 0x0004
			};

			friend class Body;
			friend class Fixture;
			friend class ContactManager;
			friend class Controller;

			void Solve(const PTimeStep& step);
			void SolveTOI(const PTimeStep& step);

			BlockAllocator m_blockAllocator;
			StackAllocator m_stackAllocator;

			s32 m_flags;

			ContactManager m_contactManager;

			Body* m_bodyList;
			Joint* m_jointList;

			s32 m_bodyCount;
			s32 m_jointCount;

			glm::vec2 m_gravity;
			bool m_allowSleep;

			DestructionListener* m_destructionListener;

			// This is used to compute the time step ratio to
			// support a variable time step.
			real32 m_inv_dt0;

			// These are for debugging the solver.
			bool m_warmStarting;
			bool m_continuousPhysics;
			bool m_subStepping;

			bool m_stepComplete;

			Profile m_profile;
		};

		inline Body* World::GetBodyList()
		{
			return m_bodyList;
		}

		inline const Body* World::GetBodyList() const
		{
			return m_bodyList;
		}

		inline Joint* World::GetJointList()
		{
			return m_jointList;
		}

		inline const Joint* World::GetJointList() const
		{
			return m_jointList;
		}

		inline Contact* World::GetContactList()
		{
			return m_contactManager.m_contactList;
		}

		inline const Contact* World::GetContactList() const
		{
			return m_contactManager.m_contactList;
		}

		inline s32 World::GetBodyCount() const
		{
			return m_bodyCount;
		}

		inline s32 World::GetJointCount() const
		{
			return m_jointCount;
		}

		inline s32 World::GetContactCount() const
		{
			return m_contactManager.m_contactCount;
		}

		inline void World::SetGravity(const glm::vec2& gravity)
		{
			m_gravity = gravity;
		}

		inline glm::vec2 World::GetGravity() const
		{
			return m_gravity;
		}

		inline bool World::IsLocked() const
		{
			return (m_flags & locked) == locked;
		}

		inline void World::SetAutoClearForces(bool flag)
		{
			if (flag)
			{
				m_flags |= clearForces;
			}
			else
			{
				m_flags &= ~clearForces;
			}
		}

		/// Get the flag that controls automatic clearing of forces after each time step.
		inline bool World::GetAutoClearForces() const
		{
			return (m_flags & clearForces) == clearForces;
		}

		inline const ContactManager& World::GetContactManager() const
		{
			return m_contactManager;
		}

		inline const Profile& World::GetProfile() const
		{
			return m_profile;
		}



	}


}