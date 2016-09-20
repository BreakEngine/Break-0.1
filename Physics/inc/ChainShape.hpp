#pragma once
#include "Shape.hpp"

namespace Break
{
	namespace Physics
	{

		class BREAK_API EdgeShape;


		/// A chain shape is a free form sequence of line segments.
		/// The chain has two-sided collision, so you can use inside and outside collision.
		/// Therefore, you may use any winding order.
		/// Since there may be many vertices, they are allocated using Alloc.
		/// Connectivity information is used to create smooth collisions.
		/// WARNING: The chain will not collide properly if there are self-intersections.
		class BREAK_API ChainShape : public Shape
		{
		public:
			ChainShape();

			/// The destructor frees the vertices using Free.
			~ChainShape();

			/// Create a loop. This automatically adjusts connectivity.
			/// @param vertices an array of vertices, these are copied
			/// @param count the vertex count
			void CreateLoop(const glm::vec2* vertices, s32 count);

			/// Create a chain with isolated end vertices.
			/// @param vertices an array of vertices, these are copied
			/// @param count the vertex count
			void CreateChain(const glm::vec2* vertices, s32 count);

			/// Establish connectivity to a vertex that precedes the first vertex.
			/// Don't call this for loops.
			void SetPrevVertex(const glm::vec2& prevVertex);

			/// Establish connectivity to a vertex that follows the last vertex.
			/// Don't call this for loops.
			void SetNextVertex(const glm::vec2& nextVertex);

			/// Implement Shape. Vertices are cloned using Alloc.
			Shape* Clone(BlockAllocator* allocator) const;

			/// @see Shape::GetChildCount
			s32 GetChildCount() const;

			/// Get a child edge.
			void GetChildEdge(EdgeShape* edge, s32 index) const;

			/// This always return false.
			/// @see Shape::TestPoint
			bool TestPoint(const Transform2D& Transform2D, const glm::vec2& p) const;

			/// Implement Shape.
			bool RayCast(RayCastOutput* output, const RayCastInput& input,const Transform2D& Transform2D, s32 childIndex) const;

			/// @see Shape::ComputeAABB
			void ComputeAABB(AABB* aabb, const Transform2D& Transform2D, s32 childIndex) const;

			/// Chains have zero mass.
			/// @see Shape::ComputeMass
			void ComputeMass(MassData* massData, real32 density) const;

			/// The vertices. Owned by this class.
			glm::vec2* m_vertices;

			/// The vertex count.
			s32 m_count;

			glm::vec2 m_prevVertex, m_nextVertex;
			bool m_hasPrevVertex, m_hasNextVertex;
		};

		inline ChainShape::ChainShape()
		{
			m_type = chain;
			m_radius = polygonRadius;
			m_vertices = NULL;
			m_count = 0;
			m_hasPrevVertex = false;
			m_hasNextVertex = false;
		}




	}
}