#pragma once

#include <Globals.hpp>

namespace Break
{
	namespace Physics
	{
		class BREAK_API PTimeStep
		{
		public:
			real64  delta, elapsedTime;

			real32 inv_dt;		// inverse time step (0 if dt == 0).
			real32 dtRatio;	// dt * inv_dt0
			s32 velocityIterations;
			s32 positionIterations;
			bool warmStarting;

			PTimeStep(real64 d=0, real64 e=0)
				:delta(d), elapsedTime(e)
			{

			}

		};
	}
}