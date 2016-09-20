#pragma once
#include "Contact2D.hpp"

namespace Break
{

	namespace Physics
	{

		class BREAK_API BlockAllocator;

		class BREAK_API ChainAndCircleContact : public Contact
		{
		public:
			static Contact* Create(	Fixture* fixtureA, s32 indexA,Fixture* fixtureB, s32 indexB, BlockAllocator* allocator);

			static void Destroy(Contact* contact, BlockAllocator* allocator);

			ChainAndCircleContact(Fixture* fixtureA, s32 indexA, Fixture* fixtureB, s32 indexB);
			~ChainAndCircleContact() {}

			void Evaluate(Manifold* manifold, const Transform2D& xfA, const Transform2D& xfB);
		};


	}
}