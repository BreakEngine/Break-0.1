//
// Created by Moustapha on 01/10/2015.
//

#ifndef BREAK_0_1_TIMESTEP_HPP
#define BREAK_0_1_TIMESTEP_HPP

#include "Globals.hpp"


namespace Break{
	namespace Infrastructure
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

		class BREAK_API TimeStep
		{
		public:
			real64  delta, elapsedTime;

			real32 inv_dt;		// inverse time step (0 if dt == 0).
			real32 dtRatio;	// dt * inv_dt0
			s32 velocityIterations;
			s32 positionIterations;
			bool warmStarting;

			TimeStep(real64 d=0, real64 e=0)
				:delta(d), elapsedTime(e)
			{

			}

		};


		//structs for physics ..

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
			TimeStep step;
			Position* positions;
			Velocity* velocities;
		};


	}
}
#endif //BREAK_0_1_TIMESTEP_HPP
