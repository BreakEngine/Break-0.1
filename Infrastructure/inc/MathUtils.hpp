#pragma once
#include "Globals.hpp"
#include "Transform2D.hpp"


namespace Break
{
	namespace Infrastructure
	{

		class BREAK_API MathUtils
		{

		public:
			static inline real32 toDegrees(real32 rad)
			{
				return rad * (180/glm::pi<real32>());
			}
			static inline real32 toRadians(real32 deg)
			{
				return deg * (glm::pi<real32>()/180);
			}


			/// This function is used to ensure that a floating point number is not a NaN or infinity.
			static inline bool IsVal(real32 x)
			{
				s32 ix = *reinterpret_cast<s32*>(&x);
				return (ix & 0x7f800000) != 0x7f800000;
			}

			static inline bool IsValid(glm::vec2 v)
			{
				return IsVal(v.x) && IsVal(v.y);
			}




			static inline real32 Distance(const glm::vec2& a, const glm::vec2& b)
			{
				return glm::distance(a,b);
				//glm::vec2 c = a - b;
				//return c.length();
			}

			static inline real32 DistanceSquared(const glm::vec2& a, const glm::vec2& b)
			{
				real32 c = glm::distance(a,b);
				return c * c;
				//glm::vec2 c = a - b;
				//return glm::dot(c, c);
			}




			/// Perform the cross product on two vectors. In 2D this produces a scalar.
			static inline real32 Cross2(const glm::vec2& a, const glm::vec2& b)
			{
				real32 res = (a.x * b.y) - (a.y * b.x);
				return res;
			}

			/// Perform the cross product on a vector and a scalar. In 2D this produces
			/// a vector.
			static inline glm::vec2 Cross2(const glm::vec2& a, real32 s)
			{
				return glm::vec2(s * a.y, -s * a.x);
			}

			/// Perform the cross product on a scalar and a vector. In 2D this produces
			/// a vector.
			static inline glm::vec2 Cross2(real32 s, const glm::vec2& a)
			{
				return glm::vec2(-s * a.y, s * a.x);
			}




			static real32 LengthSquared(glm::vec2 v) 
			{
				return  (v.length() * v.length());
			}



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




			static inline glm::vec2 Mul(const Transform2D& T, const glm::vec2& v)
			{
				real32 x = (T.q.c * v.x - T.q.s * v.y) + T.p.x;
				real32 y = (T.q.s * v.x + T.q.c * v.y) + T.p.y;

				return glm::vec2(x, y);
			}

			static inline glm::vec2 MulT(const Transform2D& T, const glm::vec2& v)
			{
				real32 px = v.x - T.p.x;
				real32 py = v.y - T.p.y;
				real32 x = (T.q.c * px + T.q.s * py);
				real32 y = (-T.q.s * px + T.q.c * py);

				return glm::vec2(x, y);
			}

			// v2 = A.q.Rot(B.q.Rot(v1) + B.p) + A.p
			//    = (A.q * B.q).Rot(v1) + A.q.Rot(B.p) + A.p
			static inline Transform2D Mul(const Transform2D& A, const Transform2D& B)
			{
				Transform2D C;
				C.q = Mul(A.q, B.q);
				C.p = Mul(A.q, B.p) + A.p;
				return C;
			}

			// v2 = A.q' * (B.q * v1 + B.p - A.p)
			//    = A.q' * B.q * v1 + A.q' * (B.p - A.p)
			static inline Transform2D MulT(const Transform2D& A, const Transform2D& B)
			{
				Transform2D C;
				C.q = MulT(A.q, B.q);
				C.p = MulT(A.q, B.p - A.p);
				return C;
			}




			/// Multiply a matrix times a vector.
			static inline glm::vec2 Mul(const glm::mat3& A, const glm::vec2& v)
			{
				return glm::vec2(A[0][0] * v.x + A[1][0] * v.y, A[0][1] * v.x + A[1][1] * v.y);
			}


			template<typename T> static inline void Swap(T& a, T& b)
			{
				T tmp = a;
				a = b;
				b = tmp;
			}


			/// Solve A * x = b, where b is a column vector. This is more efficient
			/// than computing the inverse in one-shot cases.
			static glm::vec3 SolveEq3(const glm::mat3& a ,const glm::vec3& b)
			{
				glm::vec3 ex = glm::vec3(a[0][0],a[0][1],a[0][2]);
				glm::vec3 ey = glm::vec3(a[1][0],a[1][1],a[1][2]);
				glm::vec3 ez = glm::vec3(a[2][0],a[2][1],a[2][2]);

				real32 det = glm::dot(ex, glm::cross(ey, ez));
				if (det != 0.0f)
				{
					det = 1.0f / det;
				}
				glm::vec3 x;
				x.x = det * glm::dot(b, glm::cross(ey, ez));
				x.y = det * glm::dot(ex, glm::cross(b, ez));
				x.z = det * glm::dot(ex, glm::cross(ey, b));
				return x;
			}


			/// Solve A * x = b, where b is a column vector. This is more efficient
			/// than computing the inverse in one-shot cases.
			static glm::vec2 SolveEq2(const glm::mat3& a ,const glm::vec2& b) 
			{
				real32 a11 = a[0][0], a12 = a[1][0], a21 = a[0][1], a22 = a[1][1];

				real32 det = a11 * a22 - a12 * a21;
				if (det != 0.0f)
				{
					det = 1.0f / det;
				}
				glm::vec2 x;
				x.x = det * (a22 * b.x - a12 * b.y);
				x.y = det * (a11 * b.y - a21 * b.x);
				return x;
			}

			/// Solve A * x = b, where b is a column vector. This is more efficient
			/// than computing the inverse in one-shot cases.
			static glm::vec2 SolveEq1(const glm::mat2& a , const glm::vec2& b) 
			{
				real32 a11 = a[0][0], a12 = a[1][0], a21 = a[0][1], a22 = a[1][1];

				real32 det = a11 * a22 - a12 * a21;
				if (det != 0.0f)
				{
					det = 1.0f / det;
				}
				glm::vec2 x;
				x.x = det * (a22 * b.x - a12 * b.y);
				x.y = det * (a11 * b.y - a21 * b.x);
				return x;
			}


			///
			static glm::mat3 GetInverseEq2(glm::mat3& in ,glm::mat3& M) 
			{
				real32 a = in[0][0], b = in[1][0], c = in[0][1], d = in[1][1];

				real32 det = a * d - b * c;
				if (det != 0.0f)
				{
					det = 1.0f / det;
				}

				M[0][0] =  det * d;	M[1][0] = -det * b; M[0][2] = 0.0f;
				M[0][1] = -det * c;	M[1][1] =  det * a; M[1][2] = 0.0f;
				M[2][0] = 0.0f; M[2][1] = 0.0f; M[2][2] = 0.0f;

				return M;
			}

			/// Returns the zero matrix if singular.
			static glm::mat3 GetSymInverseEq3(glm::mat3& in ,glm::mat3& M) 
			{
				glm::vec3 ex  = glm::vec3(in[0][0],in[0][1],in[0][2]);
				glm::vec3 ey  = glm::vec3(in[1][0],in[1][1],in[1][2]);
				glm::vec3 ez  = glm::vec3(in[2][0],in[2][1],in[2][2]);

				real32 det = glm::dot(ex, glm::cross(ey, ez));
				if (det != 0.0f)
				{
					det = 1.0f / det;
				}

				real32 a11 = ex.x, a12 = ey.x, a13 = ez.x;
				real32 a22 = ey.y, a23 = ez.y;
				real32 a33 = ez.z;

				M[0][0] = det * (a22 * a33 - a23 * a23);
				M[0][1] = det * (a13 * a23 - a12 * a33);
				M[0][2] = det * (a12 * a23 - a13 * a22);

				M[1][0] = M[0][1];
				M[1][1] = det * (a11 * a33 - a13 * a13);
				M[1][2] = det * (a13 * a12 - a11 * a23);

				M[2][0] = M[0][2];
				M[2][1] = M[1][2];
				M[2][2] = det * (a11 * a22 - a12 * a12);

				return M;
			}



		};


		//----------------inline funcs from Transform..
		inline void Sweep::GetTransform(Transform2D* xf, real32 beta) const 
		{
			xf->p = (1.0f - beta) * c0 + beta * c;
			real32 angle = (1.0f - beta) * a0 + beta * a;
			xf->q.Set(angle);

			// Shift to origin
			xf->p -= MathUtils::Mul(xf->q, localCenter);
		}

		inline void Sweep::Advance(real32 alpha)
		{
			assert(alpha0 < 1.0f);
			real32 beta = (alpha - alpha0) / (1.0f - alpha0);
			c0 += beta * (c - c0);
			a0 += beta * (a - a0);
			alpha0 = alpha;
		}

		/// Normalize an angle in radians to be between -pi and pi
		inline void Sweep::Normalize()
		{
			real32 twoPi = 2.0f * glm::pi<float>();
			real32 d =  twoPi * floorf(a0 / twoPi);
			a0 -= d;
			a -= d;
		}




	}
}