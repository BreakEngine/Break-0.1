#pragma once
#include "Body2D.hpp"
#include "Collision.hpp"
#include "Shape.hpp"
#include <MathUtils.hpp>
namespace Break
{
	namespace Physics
	{


		class BREAK_API BlockAllocator;
		class BREAK_API Body;
		class BREAK_API BroadPhase;
		class BREAK_API Fixture;

		/// This holds contact filtering data.
		struct BREAK_API Filter
		{
			Filter()
			{
				categoryBits = 0x0001;
				maskBits = 0xFFFF;
				groupIndex = 0;
			}

			/// The collision category bits. Normally you would just set one bit.
			u16 categoryBits;

			/// The collision mask bits. This states the categories that this
			/// shape would accept for collision.
			u16 maskBits;

			/// Collision groups allow a certain group of objects to never collide (negative)
			/// or always collide (positive). Zero means no collision group. Non-zero group
			/// filtering always wins against the mask bits.
			s16 groupIndex;
		};

		/// A fixture definition is used to create a fixture. This class defines an
		/// abstract fixture definition. You can reuse fixture definitions safely.
		struct BREAK_API FixtureDef
		{
			/// The constructor sets the default fixture definition values.
			FixtureDef()
			{
				shape = NULL;
				userData = NULL;
				friction = 0.2f;
				restitution = 0.0f;
				density = 0.0f;
				isSensor = false;
			}

			/// The shape, this must be set. The shape will be cloned, so you
			/// can create the shape on the stack.
			const Shape* shape;

			/// Use this to store application specific fixture data.
			void* userData;

			/// The friction coefficient, usually in the range [0,1].
			real32 friction;

			/// The restitution (elasticity) usually in the range [0,1].
			real32 restitution;

			/// The density, usually in kg/m^2.
			real32 density;

			/// A sensor shape collects contact information but never generates a collision
			/// response.
			bool isSensor;

			/// Contact filtering data.
			Filter filter;
		};

		/// This proxy is used internally to connect fixtures to the broad-phase.
		struct BREAK_API FixtureProxy
		{
			AABB aabb;
			Fixture* fixture;
			s32 childIndex;
			s32 proxyId;
		};

		/// A fixture is used to attach a shape to a body for collision detection. A fixture
		/// inherits its Transform2D from its parent. Fixtures hold additional non-geometric data
		/// such as friction, collision filters, etc.
		/// Fixtures are created via Body::CreateFixture.
		/// @warning you cannot reuse fixtures.
		class BREAK_API Fixture
		{
		public:
			/// Get the type of the child shape. You can use this to down cast to the concrete shape.
			/// @return the shape type.
			Shape::Type GetType() const;

			/// Get the child shape. You can modify the child shape, however you should not change the
			/// number of vertices because this will crash some collision caching mechanisms.
			/// Manipulating the shape may lead to non-physical behavior.
			Shape* GetShape();
			const Shape* GetShape() const;

			/// Set if this fixture is a sensor.
			void SetSensor(bool sensor);

			/// Is this fixture a sensor (non-solid)?
			/// @return the true if the shape is a sensor.
			bool IsSensor() const;

			/// Set the contact filtering data. This will not update contacts until the next time
			/// step when either parent body is active and awake.
			/// This automatically calls Refilter.
			void SetFilterData(const Filter& filter);

			/// Get the contact filtering data.
			const Filter& GetFilterData() const;

			/// Call this if you want to establish collision that was previously disabled by ContactFilter::ShouldCollide.
			void Refilter();

			/// Get the parent body of this fixture. This is NULL if the fixture is not attached.
			/// @return the parent body.
			Body* GetBody();
			const Body* GetBody() const;

			/// Get the next fixture in the parent body's fixture list.
			/// @return the next shape.
			Fixture* GetNext();
			const Fixture* GetNext() const;

			/// Get the user data that was assigned in the fixture definition. Use this to
			/// store your application specific data.
			void* GetUserData() const;

			/// Set the user data. Use this to store your application specific data.
			void SetUserData(void* data);

			/// Test a point for containment in this fixture.
			/// @param p a point in world coordinates.
			bool TestPoint(const glm::vec2& p) const;

			/// Cast a ray against this shape.
			/// @param output the ray-cast results.
			/// @param input the ray-cast input parameters.
			bool RayCast(RayCastOutput* output, const RayCastInput& input, s32 childIndex) const;

			/// Get the mass data for this fixture. The mass data is based on the density and
			/// the shape. The rotational inertia is about the shape's origin. This operation
			/// may be expensive.
			void GetMassData(MassData* massData) const;

			/// Set the density of this fixture. This will _not_ automatically adjust the mass
			/// of the body. You must call Body::ResetMassData to update the body's mass.
			void SetDensity(real32 density);

			/// Get the density of this fixture.
			real32 GetDensity() const;

			/// Get the coefficient of friction.
			real32 GetFriction() const;

			/// Set the coefficient of friction. This will _not_ change the friction of
			/// existing contacts.
			void SetFriction(real32 friction);

			/// Get the coefficient of restitution.
			real32 GetRestitution() const;

			/// Set the coefficient of restitution. This will _not_ change the restitution of
			/// existing contacts.
			void SetRestitution(real32 restitution);

			/// Get the fixture's AABB. This AABB may be enlarge and/or stale.
			/// If you need a more accurate AABB, compute it using the shape and
			/// the body Transform2D.
			const AABB& GetAABB(s32 childIndex) const;

			/// Dump this fixture to the log file.
			void Dump(s32 bodyIndex);

		protected:

			friend class Body;
			friend class World;
			friend class Contact;
			friend class ContactManager;

			Fixture();

			// We need separation create/destroy functions from the constructor/destructor because
			// the destructor cannot access the allocator (no destructor arguments allowed by C++).
			void Create(BlockAllocator* allocator, Body* body, const FixtureDef* def);
			void Destroy(BlockAllocator* allocator);

			// These support body activation/deactivation.
			void CreateProxies(BroadPhase* broadPhase, const Transform2D& xf);
			void DestroyProxies(BroadPhase* broadPhase);

			void Synchronize(BroadPhase* broadPhase, const Transform2D& xf1, const Transform2D& xf2);

			real32 m_density;

			Fixture* m_next;
			Body* m_body;

			Shape* m_shape;

			real32 m_friction;
			real32 m_restitution;

			FixtureProxy* m_proxies;
			s32 m_proxyCount;

			Filter m_filter;

			bool m_isSensor;

			void* m_userData;
		};

		inline Shape::Type Fixture::GetType() const
		{
			return m_shape->GetType();
		}

		inline Shape* Fixture::GetShape()
		{
			return m_shape;
		}

		inline const Shape* Fixture::GetShape() const
		{
			return m_shape;
		}

		inline bool Fixture::IsSensor() const
		{
			return m_isSensor;
		}

		inline const Filter& Fixture::GetFilterData() const
		{
			return m_filter;
		}

		inline void* Fixture::GetUserData() const
		{
			return m_userData;
		}

		inline void Fixture::SetUserData(void* data)
		{
			m_userData = data;
		}

		inline Body* Fixture::GetBody()
		{
			return m_body;
		}

		inline const Body* Fixture::GetBody() const
		{
			return m_body;
		}

		inline Fixture* Fixture::GetNext()
		{
			return m_next;
		}

		inline const Fixture* Fixture::GetNext() const
		{
			return m_next;
		}

		inline void Fixture::SetDensity(real32 density)
		{
			assert(Infrastructure::MathUtils::IsVal(density) && density >= 0.0f);
			m_density = density;
		}

		inline real32 Fixture::GetDensity() const
		{
			return m_density;
		}

		inline real32 Fixture::GetFriction() const
		{
			return m_friction;
		}

		inline void Fixture::SetFriction(real32 friction)
		{
			m_friction = friction;
		}

		inline real32 Fixture::GetRestitution() const
		{
			return m_restitution;
		}

		inline void Fixture::SetRestitution(real32 restitution)
		{
			m_restitution = restitution;
		}

		inline bool Fixture::TestPoint(const glm::vec2& p) const
		{
			return m_shape->TestPoint(m_body->GetTransform2D(), p);
		}

		inline bool Fixture::RayCast(RayCastOutput* output, const RayCastInput& input, s32 childIndex) const
		{
			return m_shape->RayCast(output, input, m_body->GetTransform2D(), childIndex);
		}

		inline void Fixture::GetMassData(MassData* massData) const
		{
			m_shape->ComputeMass(massData, m_density);
		}

		inline const AABB& Fixture::GetAABB(s32 childIndex) const
		{
			assert(0 <= childIndex && childIndex < m_proxyCount);
			return m_proxies[childIndex].aabb;
		}




	}
}