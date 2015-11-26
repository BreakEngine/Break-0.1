//
// Created by Moustapha on 01/10/2015.
//

#ifndef BREAK_0_1_TIMEMANAGER_HPP
#define BREAK_0_1_TIMEMANAGER_HPP

#include "Globals.hpp"
#include "TimeStep.hpp"


namespace Break{
	namespace Infrastructure
	{
		static  real64  s_invFrequency = 0;
		class BREAK_API Time
		{
		public:
			///types of the time FPS class
			enum Type { UNLIMITED, LIMITED };
		public:
			///determines the number of frames per second
			unsigned int _frameLimit;
			///register the last time
			double _lastTime;
			///count time between frames
			double _counter;
			///frame counter of frames per second
			unsigned int _frameCounter;
			///tmp var to store time and reset every second
			double _secondTick;
			///total elapsed time
			double _totalElapsedTime;
			///last timeStep
			TimeStep _previousStep;

			///the FPS
			unsigned int FPS;
			///type of FPS
			Type _type;

			real64 m_start;
		public:

			Time()
			{
				_type = Type::LIMITED;
				_frameLimit = 60;
				_lastTime = 0;
				_counter = 0;
				_secondTick = 0;
				_frameCounter = 0;
				_totalElapsedTime = 0;
				FPS = 0;
				_previousStep = TimeStep(0,0);
				
				LARGE_INTEGER largeInteger;

				if (s_invFrequency == 0.0f)
				{ 
					QueryPerformanceFrequency(&largeInteger);
					s_invFrequency = real64(largeInteger.QuadPart);
					if (s_invFrequency > 0.0f)
					{
						s_invFrequency = 1000.0f / s_invFrequency;
					}
				}

				QueryPerformanceCounter(&largeInteger);
				m_start = real64(largeInteger.QuadPart);
			}

			TimeStep getTimeStep()
			{
				return _previousStep;
			}

			Type getType()
			{
				return _type;
			}

			void setType(Type val)
			{
				_type = val;
			}

			u32 getFPS(){
				return FPS;
			}

			void setFrameLimit(u32 val)
			{
				if(val>0)
					_frameLimit = val;
			}

			u32 getFrameLimit()
			{
				return _frameLimit;
			}

			void Reset()
			{
				LARGE_INTEGER largeInteger;
				QueryPerformanceCounter(&largeInteger);
				m_start = real64(largeInteger.QuadPart);
			}

			/// Get the time since construction or the last reset.
			real32 GetMilliseconds() const
			{
				LARGE_INTEGER largeInteger;
				QueryPerformanceCounter(&largeInteger);
				real64 count = real64(largeInteger.QuadPart);
				real32 ms = real32(s_invFrequency * (count - m_start));
				return ms;
			}

		};
	}
}
#endif //BREAK_0_1_TIMEMANAGER_HPP
