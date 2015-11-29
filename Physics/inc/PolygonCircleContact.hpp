#pragma once
#include "Contact2D.hpp"

namespace Break
{
	namespace Physics
	{


		class BREAK_API BlockAllocator;

		class BREAK_API PolygonAndCircleContact : public Contact
		{
		public:
			static Contact* Create(Fixture* fixtureA, s32 indexA, Fixture* fixtureB, s32 indexB, BlockAllocator* allocator);
			static void Destroy(Contact* contact, BlockAllocator* allocator);

			PolygonAndCircleContact(Fixture* fixtureA, Fixture* fixtureB);
			~PolygonAndCircleContact() {}

			void Evaluate(Manifold* manifold, const Transform2D& xfA, const Transform2D& xfB);
		};


	}
}