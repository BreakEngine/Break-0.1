#pragma once

#include "Break2D.hpp"
#include "Rect.hpp"
#include <World2D.hpp>
#include "Fixture.hpp"
#include "CircleShape.hpp"

namespace Break
{

	namespace Physics
	{


		class BREAK_API CircleBody
		{
		public:
			CircleBody(World *world,Infrastructure::Rect position , int radius,  bool isDynamic);
			~CircleBody();



			Infrastructure::Rect GetRect();
			double GetAngle();
			glm::vec2 GetOrigin();


			Body *Physics;
		private:

			World *_world;
			Infrastructure::Rect R;

			BodyDef bodyDef;
			FixtureDef fixturedef;

			CircleShape circleshape;
		};


	}



}