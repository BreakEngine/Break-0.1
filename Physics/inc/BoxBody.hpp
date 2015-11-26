#pragma once

#include "Break2D.hpp"
#include "Rect.hpp"

namespace Break
{
	namespace physics
	{

		class BREAK_API BoxBody
		{
		public:
			BoxBody(){}
			BoxBody(World *world,Graphics::Rect position ,  bool isDynamic); 
			~BoxBody();

			Graphics::Rect GetRect();

			double GetAngle();

			void SetAngle(real32 angle);

			glm::vec2 GetOrigin();

			FixtureDef GetFixture()
			{
				return fixturedef;
			}

			BodyDef GetBodyDef()
			{
				return bodyDef;
			}

			Body *Physics;

		private:

			World *_world;
			Graphics::Rect R;

			BodyDef bodyDef;
			FixtureDef fixturedef;
			PolygonShape polyshape;
		};
	}
}