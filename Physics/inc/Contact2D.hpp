#pragma once
#include "MathUtils.hpp"
#include "Collision.hpp"
#include "Shape.hpp"
#include "Fixture.hpp"

namespace Break
{
	namespace Physics
	{

		class BREAK_API Body;
		class BREAK_API Contact;
		class BREAK_API Fixture;
		class BREAK_API World;
		class BREAK_API BlockAllocator;
		class BREAK_API StackAllocator;
		class BREAK_API ContactListener;

		/// Friction mixing law. The idea is to allow either fixture to drive the restitution to zero.
		/// For example, anything slides on ice.
		inline real32 MixFriction(real32 friction1, real32 friction2)
		{
			return sqrtf(friction1 * friction2);
		}

		/// Restitution mixing law. The idea is allow for anything to bounce off an inelastic surface.
		/// For example, a superball bounces on anything.
		inline real32 MixRestitution(real32 restitution1, real32 restitution2)
		{
			return restitution1 > restitution2 ? restitution1 : restitution2;
		}

		typedef Contact* ContactCreateFcn(	Fixture* fixtureA, s32 indexA,Fixture* fixtureB, s32 indexB,BlockAllocator* allocator);

		typedef void ContactDestroyFcn(Contact* contact, BlockAllocator* allocator);

		struct BREAK_API ContactRegister
		{
			ContactCreateFcn* createFcn;
			ContactDestroyFcn* destroyFcn;
			bool primary;
		};

		/// A contact edge is used to connect bodies and contacts together
		/// in a contact graph where each body is a node and each contact
		/// is an edge. A contact edge belongs to a doubly linked list
		/// maintained in each attached body. Each contact has two contact
		/// nodes, one for each attached body.
		struct BREAK_API ContactEdge
		{
			Body* other;			///< provides quick access to the other body attached.
			Contact* contact;		///< the contact
			ContactEdge* prev;	///< the previous contact edge in the body's contact list
			ContactEdge* next;	///< the next contact edge in the body's contact list
		};

		/// The class manages contact between two shapes. A contact exists for each overlapping
		/// AABB in the broad-phase (except if filtered). Therefore a contact object may exist
		/// that has no contact points.
		class BREAK_API Contact
		{ 
		public:

			/// Get the contact manifold. Do not modify the manifold unless you understand the
			/// internals of Box2D.
			Manifold* GetManifold();
			const Manifold* GetManifold() const;

			/// Get the world manifold.
			void GetWorldManifold(WorldManifold* worldManifold) const;

			/// Is this contact touching?
			bool IsTouching() const;

			/// Enable/disable this contact. This can be used inside the pre-solve
			/// contact listener. The contact is only disabled for the current
			/// time step (or sub-step in continuous collisions).
			void SetEnabled(bool flag);

			/// Has this contact been disabled?
			bool IsEnabled() const;

			/// Get the next contact in the world's contact list.
			Contact* GetNext();
			const Contact* GetNext() const;

			/// Get fixture A in this contact.
			Fixture* GetFixtureA();
			const Fixture* GetFixtureA() const;

			/// Get the child primitive index for fixture A.
			s32 GetChildIndexA() const;

			/// Get fixture B in this contact.
			Fixture* GetFixtureB();
			const Fixture* GetFixtureB() const;

			/// Get the child primitive index for fixture B.
			s32 GetChildIndexB() const;

			/// Override the default friction mixture. You can call this in ContactListener::PreSolve.
			/// This value persists until set or reset.
			void SetFriction(real32 friction);

			/// Get the friction.
			real32 GetFriction() const;

			/// Reset the friction mixture to the default value.
			void ResetFriction();

			/// Override the default restitution mixture. You can call this in ContactListener::PreSolve.
			/// The value persists until you set or reset.
			void SetRestitution(real32 restitution);

			/// Get the restitution.
			real32 GetRestitution() const;

			/// Reset the restitution to the default value.
			void ResetRestitution();

			/// Set the desired tangent speed for a conveyor belt behavior. In meters per second.
			void SetTangentSpeed(real32 speed);

			/// Get the desired tangent speed. In meters per second.
			real32 GetTangentSpeed() const;

			/// Evaluate this contact with your own manifold and Transform2Ds.
			virtual void Evaluate(Manifold* manifold, const Transform2D& xfA, const Transform2D& xfB) = 0;

		protected:
			friend class ContactManager;
			friend class World;
			friend class ContactSolver;
			friend class Body;
			friend class Fixture;

			// Flags stored in m_flags
			enum
			{
				// Used when crawling contact graph when forming islands.
				islandFlag		= 0x0001,

				// Set when the shapes are touching.
				touchingFlag		= 0x0002,

				// This contact can be disabled (by user)
				enabledFlag		= 0x0004,

				// This contact needs filtering because a fixture filter was changed.
				filterFlag		= 0x0008,

				// This bullet contact had a TOI event
				bulletHitFlag		= 0x0010,

				// This contact has a valid TOI in m_toi
				toiFlag			= 0x0020
			};

			/// Flag this contact for filtering. Filtering will occur the next time step.
			void FlagForFiltering();

			static void AddType(ContactCreateFcn* createFcn, ContactDestroyFcn* destroyFcn,Shape::Type typeA, Shape::Type typeB);
			static void InitializeRegisters();
			static Contact* Create(Fixture* fixtureA, s32 indexA, Fixture* fixtureB, s32 indexB, BlockAllocator* allocator);
			static void Destroy(Contact* contact, Shape::Type typeA, Shape::Type typeB, BlockAllocator* allocator);
			static void Destroy(Contact* contact, BlockAllocator* allocator);

			Contact() : m_fixtureA(NULL), m_fixtureB(NULL) {}
			Contact(Fixture* fixtureA, s32 indexA, Fixture* fixtureB, s32 indexB);
			virtual ~Contact() {}

			void Update(ContactListener* listener);

			static ContactRegister s_registers[Shape::typeCount][Shape::typeCount];
			static bool s_initialized;

			u32 m_flags;

			// World pool and list pointers.
			Contact* m_prev;
			Contact* m_next;

			// Nodes for connecting bodies.
			ContactEdge m_nodeA;
			ContactEdge m_nodeB;

			Fixture* m_fixtureA;
			Fixture* m_fixtureB;

			s32 m_indexA;
			s32 m_indexB;

			Manifold m_manifold;

			s32 m_toiCount;
			real32 m_toi;

			real32 m_friction;
			real32 m_restitution;

			real32 m_tangentSpeed;
		};

		inline Manifold* Contact::GetManifold()
		{
			return &m_manifold;
		}

		inline const Manifold* Contact::GetManifold() const
		{
			return &m_manifold;
		}

		inline void Contact::GetWorldManifold(WorldManifold* worldManifold) const
		{
			const Body* bodyA = m_fixtureA->GetBody();
			const Body* bodyB = m_fixtureB->GetBody();
			const Shape* shapeA = m_fixtureA->GetShape();
			const Shape* shapeB = m_fixtureB->GetShape();

			worldManifold->Initialize(&m_manifold, bodyA->GetTransform2D(), shapeA->m_radius, bodyB->GetTransform2D(), shapeB->m_radius);
		}

		inline void Contact::SetEnabled(bool flag)
		{
			if (flag)
			{
				m_flags |= enabledFlag;
			}
			else
			{
				m_flags &= ~enabledFlag;
			}
		}

		inline bool Contact::IsEnabled() const
		{
			return (m_flags & enabledFlag) == enabledFlag;
		}

		inline bool Contact::IsTouching() const
		{
			return (m_flags & touchingFlag) == touchingFlag;
		}

		inline Contact* Contact::GetNext()
		{
			return m_next;
		}

		inline const Contact* Contact::GetNext() const
		{
			return m_next;
		}

		inline Fixture* Contact::GetFixtureA()
		{
			return m_fixtureA;
		}

		inline const Fixture* Contact::GetFixtureA() const
		{
			return m_fixtureA;
		}

		inline Fixture* Contact::GetFixtureB()
		{
			return m_fixtureB;
		}

		inline s32 Contact::GetChildIndexA() const
		{
			return m_indexA;
		}

		inline const Fixture* Contact::GetFixtureB() const
		{
			return m_fixtureB;
		}

		inline s32 Contact::GetChildIndexB() const
		{
			return m_indexB;
		}

		inline void Contact::FlagForFiltering()
		{
			m_flags |= filterFlag;
		}

		inline void Contact::SetFriction(real32 friction)
		{
			m_friction = friction;
		}

		inline real32 Contact::GetFriction() const
		{
			return m_friction;
		}

		inline void Contact::ResetFriction()
		{
			m_friction = MixFriction(m_fixtureA->m_friction, m_fixtureB->m_friction);
		}

		inline void Contact::SetRestitution(real32 restitution)
		{
			m_restitution = restitution;
		}

		inline real32 Contact::GetRestitution() const
		{
			return m_restitution;
		}

		inline void Contact::ResetRestitution()
		{
			m_restitution = MixRestitution(m_fixtureA->m_restitution, m_fixtureB->m_restitution);
		}

		inline void Contact::SetTangentSpeed(real32 speed)
		{
			m_tangentSpeed = speed;
		}

		inline real32 Contact::GetTangentSpeed() const
		{
			return m_tangentSpeed;
		}



	}
}