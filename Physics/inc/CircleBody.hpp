#pragma once

#include "Break2D.hpp"
#include "Rect.hpp"

namespace Break
{

	namespace physics
	{


		class BREAK_API CircleBody
		{
		public:
			CircleBody(World *world,Graphics::Rect position , int radius,  bool isDynamic);
			~CircleBody();



			Graphics::Rect GetRect();
			double GetAngle();
			glm::vec2 GetOrigin();


			Body *Physics;
		private:

			World *_world;
			Graphics::Rect R;

			BodyDef bodyDef;
			FixtureDef fixturedef;

			CircleShape circleshape;
		};


	}



}