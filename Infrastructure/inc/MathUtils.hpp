//
// Created by Moustapha on 07/10/2015.
//

#ifndef BREAK_0_1_MATHUTILS_HPP
#define BREAK_0_1_MATHUTILS_HPP

#include "Globals.hpp"
#include <glm/common.hpp>
#include <glm/glm.hpp>

namespace Break{
    namespace Infrastructure{
        class BREAK_API MathUtils{
        public:

            static inline real32 toDegrees(real32 rad)
            {
                return rad * (180/PI);
            }
            static inline real32 toRadians(real32 deg)
            {
                return deg * (PI/180);
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
    }
}
#endif //BREAK_0_1_MATHUTILS_HPP
