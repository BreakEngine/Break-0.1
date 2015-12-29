#pragma once

#include "Break2D.hpp"
#include "Rect.hpp"
#include <glm/glm.hpp>
#include "Body2D.hpp"
#include "Fixture.hpp"
#include "PolygonShape.hpp"

namespace Break
{
	namespace Physics
	{
		class World;

		class BREAK_API BoxBody
		{
		public:
			BoxBody(){}
			BoxBody(World *world, Infrastructure::Rect position ,  bool isDynamic); 
			~BoxBody();

			Infrastructure::Rect GetRect();

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
			Infrastructure::Rect R;

			BodyDef bodyDef;
			FixtureDef fixturedef;
			PolygonShape polyshape;
		};
	}
}