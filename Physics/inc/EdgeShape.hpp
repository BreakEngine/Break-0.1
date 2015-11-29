#pragma once
#include "Shape.hpp"

namespace Break
{

	namespace Physics
	{

		/// A line segment (edge) shape. These can be connected in chains or loops
		/// to other edge shapes. The connectivity information is used to ensure
		/// correct contact normals.
		class BREAK_API EdgeShape : public Shape
		{
		public:
			EdgeShape();

			/// Set this as an isolated edge.
			void Set(const glm::vec2& v1, const glm::vec2& v2);

			/// Implement Shape.
			Shape* Clone(BlockAllocator* allocator) const;

			/// @see Shape::GetChildCount
			s32 GetChildCount() const;

			/// @see Shape::TestPoint
			bool TestPoint(const Transform2D& Transform2D, const glm::vec2& p) const;

			/// Implement Shape.
			bool RayCast(RayCastOutput* output, const RayCastInput& input,const Transform2D& Transform2D, s32 childIndex) const;

			/// @see Shape::ComputeAABB
			void ComputeAABB(AABB* aabb, const Transform2D& Transform2D, s32 childIndex) const;

			/// @see Shape::ComputeMass
			void ComputeMass(MassData* massData, real32 density) const;

			/// These are the edge vertices
			glm::vec2 m_vertex1, m_vertex2;

			/// Optional adjacent vertices. These are used for smooth collision.
			glm::vec2 m_vertex0, m_vertex3;
			bool m_hasVertex0, m_hasVertex3;
		};

		inline EdgeShape::EdgeShape()
		{
			m_type = edge;
			m_radius = polygonRadius;
			m_vertex0.x = 0.0f;
			m_vertex0.y = 0.0f;
			m_vertex3.x = 0.0f;
			m_vertex3.y = 0.0f;
			m_hasVertex0 = false;
			m_hasVertex3 = false;
		}

	}

}