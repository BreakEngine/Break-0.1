#pragma once

#include <Globals.hpp>
#include "PTimeStep.hpp"

namespace Break
{
	namespace Physics
	{
		/// Profiling data. Times are in milliseconds.
		struct BREAK_API Profile
		{
			real32 step;
			real32 collide;
			real32 solve;
			real32 solveInit;
			real32 solveVelocity;
			real32 solvePosition;
			real32 broadphase;
			real32 solveTOI;
		};

		/// This is an internal structure.
		struct BREAK_API Position
		{
			glm::vec2 c;
			real32 a;
		};

		/// This is an internal structure.
		struct BREAK_API Velocity
		{
			glm::vec2 v;
			real32 w;
		};

		/// Solver Data
		struct BREAK_API SolverData
		{
			PTimeStep step;
			Position* positions;
			Velocity* velocities;
		};
	}
}