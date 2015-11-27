#pragma once

#include "Break2D.hpp"
#include "BoxBody.hpp"

namespace Break
{

	namespace physics
	{

		class BREAK_API ChainBody
		{
		public:
			ChainBody(World *world,Body *ground);
			~ChainBody();


			std::vector< BoxBody *> box;
		private:
			Body *ground;

		};

	}

}