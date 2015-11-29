#pragma once
#include "Shape.hpp"
#include "PhysicsGlobals.hpp"

namespace Break
{
	namespace Physics
	{

		/// A convex polygon. It is assumed that the interior of the polygon is to
		/// the left of each edge.
		/// Polygons have a maximum number of vertices equal to ((maxPolygonVertices)).
		/// In most cases you should not need many vertices for a convex polygon.
		class BREAK_API PolygonShape : public Shape
		{
		public:
			PolygonShape();

			/// Implement Shape.
			Shape* Clone(BlockAllocator* allocator) const;

			/// @see Shape::GetChildCount
			s32 GetChildCount() const;

			/// Create a convex hull from the given array of local points.
			/// The count must be in the range [3, _maxPolygonVertices].
			/// @warning the points may be re-ordered, even if they form a convex polygon
			/// @warning collinear points are handled but not removed. Collinear points
			/// may lead to poor stacking behavior.
			void Set(const glm::vec2* points, s32 count);


			/// Build vertices to represent an axis-aligned box centered on the local origin.
			//build main structure of body relative to local origin..
			/// @param hx the half-width.
			/// @param hy the half-height.
			void SetAsBox(real32 hx, real32 hy);


			/// Build vertices to represent an oriented box.
			/// @param hx the half-width.
			/// @param hy the half-height.
			/// @param center the center of the box in local coordinates.
			/// @param angle the rotation of the box in local coordinates.
			void SetAsBox(real32 hx, real32 hy, const glm::vec2& center, real32 angle);


			/// @see Shape::TestPoint
			bool TestPoint(const Transform2D& Transform2D, const glm::vec2& p) const;


			/// Implement Shape.
			bool RayCast(RayCastOutput* output, const RayCastInput& input,const Transform2D& Transform2D, s32 childIndex) const;

			/// @see Shape::ComputeAABB
			///AABB algorithm..
			void ComputeAABB(AABB* aabb, const Transform2D& Transform2D, s32 childIndex) const;

			/// @see Shape::ComputeMass
			void ComputeMass(MassData* massData, real32 density) const;

			/// Get the vertex count.
			s32 GetVertexCount() const { return m_count; }

			/// Get a vertex by index.
			const glm::vec2& GetVertex(s32 index) const;

			/// Validate convexity. This is a very time consuming operation.
			/// @returns true if valid
			bool Validate() const;

			glm::vec2 m_centroid;
			glm::vec2 m_vertices[maxPolygonVertices];
			glm::vec2 m_normals[maxPolygonVertices];
			s32 m_count;
		};

		inline PolygonShape::PolygonShape()
		{
			m_type = polygon;
			m_radius = polygonRadius;
			m_count = 0;
			m_centroid.x = 0; m_centroid.y = 0;
		}

		inline const glm::vec2& PolygonShape::GetVertex(s32 index) const
		{
			assert(0 <= index && index < m_count);
			return m_vertices[index];
		}

	}

}
