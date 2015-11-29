#pragma once
#include "Contact2D.hpp"

namespace Break
{

	namespace Physics
	{


		class BREAK_API BlockAllocator;

		class BREAK_API PolygonContact : public Contact
		{
		public:
			static Contact* Create(	Fixture* fixtureA, s32 indexA,Fixture* fixtureB, s32 indexB, BlockAllocator* allocator);
			static void Destroy(Contact* contact, BlockAllocator* allocator);

			PolygonContact(Fixture* fixtureA, Fixture* fixtureB);
			~PolygonContact() {}

			void Evaluate(Manifold* manifold, const Transform2D& xfA, const Transform2D& xfB);
		};


	}
}