#pragma once

#include <Globals.hpp>
#include <glm/glm.hpp>
#include "Rotation2D.hpp"

namespace Break
{
	namespace Physics
	{
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
				C.q = Rotation2D::Mul(A.q, B.q);
				C.p = Rotation2D::Mul(A.q, B.p) + A.p;
				return C;
			}

			// v2 = A.q' * (B.q * v1 + B.p - A.p)
			//    = A.q' * B.q * v1 + A.q' * (B.p - A.p)
			static inline Transform2D MulT(const Transform2D& A, const Transform2D& B)
			{
				Transform2D C;
				C.q = Rotation2D::MulT(A.q, B.q);
				C.p = Rotation2D::MulT(A.q, B.p - A.p);
				return C;
			}
		};
	}
}