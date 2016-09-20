#pragma once

#include <Globals.hpp>
#include <glm/glm.hpp>

namespace Break
{
	namespace Physics
	{
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

			/// Multiply two rotations: q * r
			static inline Rotation2D Mul(const Rotation2D& q, const Rotation2D& r)
			{
				// [qc -qs] * [rc -rs] = [qc*rc-qs*rs -qc*rs-qs*rc]
				// [qs  qc]   [rs  rc]   [qs*rc+qc*rs -qs*rs+qc*rc]
				// s = qs * rc + qc * rs
				// c = qc * rc - qs * rs
				Rotation2D qr;
				qr.s = q.s * r.c + q.c * r.s;
				qr.c = q.c * r.c - q.s * r.s;
				return qr;
			}

			/// Transpose multiply two rotations: qT * r
			static inline Rotation2D MulT(const Rotation2D& q, const Rotation2D& r)
			{
				// [ qc qs] * [rc -rs] = [qc*rc+qs*rs -qc*rs+qs*rc]
				// [-qs qc]   [rs  rc]   [-qs*rc+qc*rs qs*rs+qc*rc]
				// s = qc * rs - qs * rc
				// c = qc * rc + qs * rs
				Rotation2D qr;
				qr.s = q.c * r.s - q.s * r.c;
				qr.c = q.c * r.c + q.s * r.s;
				return qr;
			}

			/// Rotate a vector
			static inline glm::vec2 Mul(const Rotation2D& q, const glm::vec2& v)
			{
				return glm::vec2(q.c * v.x - q.s * v.y, q.s * v.x + q.c * v.y);
			}

			/// Inverse rotate a vector
			static inline glm::vec2 MulT(const Rotation2D& q, const glm::vec2& v)
			{
				return glm::vec2(q.c * v.x + q.s * v.y, -q.s * v.x + q.c * v.y);
			}
		};
	}
}