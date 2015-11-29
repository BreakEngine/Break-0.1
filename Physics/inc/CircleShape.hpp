#pragma once
#include "Shape.hpp"

namespace Break
{
	namespace Physics
	{

		/// A circle shape.
		class BREAK_API CircleShape : public Shape
		{
		public:
			CircleShape();

			/// Implement Shape.
			Shape* Clone(BlockAllocator* allocator) const;

			/// @see Shape::GetChildCount
			s32 GetChildCount() const;

			/// Implement Shape.
			bool TestPoint(const Transform2D& Transform2D, const glm::vec2& p) const;

			/// Implement Shape.
			bool RayCast(RayCastOutput* output, const RayCastInput& input,const Transform2D& Transform2D, s32 childIndex) const;

			/// @see Shape::ComputeAABB
			void ComputeAABB(AABB* aabb, const Transform2D& Transform2D, s32 childIndex) const;

			/// @see Shape::ComputeMass
			void ComputeMass(MassData* massData, real32 density) const;

			/// Get the supporting vertex index in the given direction.
			s32 GetSupport(const glm::vec2& d) const;

			/// Get the supporting vertex in the given direction.
			const glm::vec2& GetSupportVertex(const glm::vec2& d) const;

			/// Get the vertex count.
			s32 GetVertexCount() const { return 1; }

			/// Get a vertex by index. Used by Distance.
			const glm::vec2& GetVertex(s32 index) const;

			/// Position
			glm::vec2 m_p;
		};

		inline CircleShape::CircleShape()
		{
			m_type = circle;
			m_radius = 0.0f;
			m_p.x = 0; m_p.y = 0; 
		}

		inline s32 CircleShape::GetSupport(const glm::vec2 &d) const
		{
			NOT_USED(d);
			return 0;
		}

		inline const glm::vec2& CircleShape::GetSupportVertex(const glm::vec2 &d) const
		{
			NOT_USED(d);
			return m_p;
		}

		inline const glm::vec2& CircleShape::GetVertex(s32 index) const
		{
			NOT_USED(index);
			assert(index == 0);
			return m_p;
		}



	}

}

