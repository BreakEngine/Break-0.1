#pragma once
#include "Globals.hpp"

namespace Break
{

	namespace Infrastructure
	{
		class MathUtils;
		/// Rotation
		class BREAK_API Rotation2D
		{
		public :
			Rotation2D() {}

			/// Initialize from an angle in radians
			explicit Rotation2D(real32 angle)
			{
				/// TODO_ERIN optimize
				s = sinf(angle);
				c = cosf(angle);
			}

			/// Set using an angle in radians.
			void Set(real32 angle)
			{
				/// TODO_ERIN optimize
				s = sinf(angle);
				c = cosf(angle);
			}

			/// Set to the identity rotation
			void SetIdentity()
			{
				s = 0.0f;
				c = 1.0f;
			}

			/// Get the angle in radians
			real32 GetAngle() const
			{
				return atan2f(s, c);
			}

			/// Get the x-axis
			glm::vec2 GetXAxis() const
			{
				return glm::vec2(c, s);
			}

			/// Get the u-axis
			glm::vec2 GetYAxis() const
			{
				return glm::vec2(-s, c);
			}

			/// Sine and cosine
			real32 s, c;
		};

		class BREAK_API Transform2D
		{
		public:
			Transform2D(){}

			Transform2D(const glm::vec2& position , const Rotation2D& rotation ) : p(position) ,q(rotation){}

			void SetIdentity()
			{
				p = glm::vec2(0.0f,0.0f);
				q.SetIdentity();
			}

			void Set(const glm::vec2& position, real32 angle)
			{
				p = position;
				q.Set(angle);
			}

			glm::vec2  p;
			Rotation2D q;
		};


		/// This describes the motion of a body/shape for TOI computation.
		/// Shapes are defined with respect to the body origin, which may
		/// no coincide with the center of mass. However, to support dynamics
		/// we must interpolate the center of mass position.
		struct BREAK_API Sweep
		{
			/// Get the interpolated transform at a specific time.
			/// @param beta is a factor in [0,1], where 0 indicates alpha0.
			void GetTransform(Transform2D* xfb, real32 beta) const;

			/// Advance the sweep forward, yielding a new initial state.
			/// @param alpha the new initial time.
			void Advance(real32 alpha);

			/// Normalize the angles.
			void Normalize();

			glm::vec2 localCenter;	///< local center of mass position
			glm::vec2 c0, c;		///< center world positions
			real32 a0, a;		   ///< world angles

			/// Fraction of the current time step in the range [0,1]
			/// c0 and a0 are the positions at alpha0.
			real32 alpha0; 
		};

	}

}