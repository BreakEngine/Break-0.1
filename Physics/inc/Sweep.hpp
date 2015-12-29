#pragma once

#include <Globals.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "Transform2D.hpp"

namespace Break
{
	namespace Physics
	{
		class BREAK_API Sweep
		{
		public:
			/// Get the interpolated transform at a specific time.
			/// @param beta is a factor in [0,1], where 0 indicates alpha0.
			void GetTransform(Transform2D* xf, real32 beta) const
			{
				xf->p = (1.0f - beta) * c0 + beta * c;
				real32 angle = (1.0f - beta) * a0 + beta * a;
				xf->q.Set(angle);

				// Shift to origin
				xf->p -= Rotation2D::Mul(xf->q, localCenter);
			}

			/// Advance the sweep forward, yielding a new initial state.
			/// @param alpha the new initial time.
			void Advance(real32 alpha)
			{
				assert(alpha0 < 1.0f);
				real32 beta = (alpha - alpha0) / (1.0f - alpha0);
				c0 += beta * (c - c0);
				a0 += beta * (a - a0);
				alpha0 = alpha;
			}

			/// Normalize the angles.
			void Normalize()
			{
				real32 twoPi = 2.0f * glm::pi<float>();
				real32 d =  twoPi * floorf(a0 / twoPi);
				a0 -= d;
				a -= d;
			}

			glm::vec2 localCenter;	///< local center of mass position
			glm::vec2 c0, c;		///< center world positions
			real32 a0, a;		   ///< world angles

			/// Fraction of the current time step in the range [0,1]
			/// c0 and a0 are the positions at alpha0.
			real32 alpha0; 
		};
	}
}