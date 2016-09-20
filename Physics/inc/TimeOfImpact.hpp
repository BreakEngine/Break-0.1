#pragma once
#include "Globals.hpp"
#include "MathUtils.hpp"
#include "Distance.hpp"
#include "Sweep.hpp"

using namespace Break::Infrastructure;

namespace Break
{

	namespace Physics
	{


		/// Input parameters for TimeOfImpact
		struct BREAK_API TOIInput
		{
			DistanceProxy proxyA;
			DistanceProxy proxyB;
			Sweep sweepA;
			Sweep sweepB;
			real32 tMax;		// defines sweep interval [0, tMax]
		};

		// Output parameters for TimeOfImpact.
		struct BREAK_API TOIOutput
		{
			enum State
			{
				unknown,
				failed,
				overlapped,
				touching,
				separated
			};

			State state;
			real32 t;
		};

		/// Compute the upper bound on time before two shapes penetrate. Time is represented as
		/// a fraction between [0,tMax]. This uses a swept separating axis and may miss some intermediate,
		/// non-tunneling collision. If you change the time interval, you should call this function
		/// again.
		/// Note: use Distance to compute the contact point and normal at the time of impact.
		void BREAK_API TimeOfImpact(TOIOutput* output, const TOIInput* input);

	}

}