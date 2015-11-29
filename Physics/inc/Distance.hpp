#pragma once
#include "Globals.hpp"
#include "MathUtils.hpp"
#include "Transform2D.hpp"

using namespace Break::Infrastructure;

namespace Break
{
	namespace Physics
	{

		class BREAK_API Shape;


		/// A distance proxy is used by the GJK algorithm.
		/// It encapsulates any shape.
		struct BREAK_API DistanceProxy
		{
			DistanceProxy() : m_vertices(NULL), m_count(0), m_radius(0.0f) {}

			/// Initialize the proxy using the given shape. The shape
			/// must remain in scope while the proxy is in use.
			void Set(const Shape* shape, s32 index);

			/// Get the supporting vertex index in the given direction.
			s32 GetSupport(const glm::vec2& d) const;

			/// Get the supporting vertex in the given direction.
			const glm::vec2& GetSupportVertex(const glm::vec2& d) const;

			/// Get the vertex count.
			s32 GetVertexCount() const;

			/// Get a vertex by index. Used by Distance.
			const glm::vec2& GetVertex(s32 index) const;

			glm::vec2 m_buffer[2];
			const glm::vec2* m_vertices;
			s32 m_count;
			real32 m_radius;
		};

		/// Used to warm start Distance.
		/// Set count to zero on first call.
		struct BREAK_API SimplexCache
		{
			real32 metric;		///< length or area
			u16 count;
			u8 indexA[3];	///< vertices on shape A
			u8 indexB[3];	///< vertices on shape B
		};

		/// Input for Distance.
		/// You have to option to use the shape radii
		/// in the computation. Even 
		struct BREAK_API DistanceInput
		{
			DistanceProxy proxyA;
			DistanceProxy proxyB;
			Transform2D Transform2DA;
			Transform2D Transform2DB;
			bool useRadii;
		};

		/// Output for Distance.
		struct BREAK_API DistanceOutput
		{
			glm::vec2 pointA;		///< closest point on shapeA
			glm::vec2 pointB;		///< closest point on shapeB
			real32 distance;
			s32 iterations;	///< number of GJK iterations used
		};

		/// Compute the closest points between two shapes. Supports any combination of:
		/// CircleShape, PolygonShape, EdgeShape. The simplex cache is input/output.
		/// On the first call set SimplexCache.count to zero.
		void BREAK_API Distance(DistanceOutput* output,SimplexCache* cache, const DistanceInput* input);


		//////////////////////////////////////////////////////////////////////////

		inline s32 DistanceProxy::GetVertexCount() const
		{
			return m_count;
		}

		inline const glm::vec2& DistanceProxy::GetVertex(s32 index) const
		{
			assert(0 <= index && index < m_count);
			return m_vertices[index];
		}

		inline s32 DistanceProxy::GetSupport(const glm::vec2& d) const
		{
			s32 bestIndex = 0;
			real32 bestValue = glm::dot(m_vertices[0], d);
			for (s32 i = 1; i < m_count; ++i)
			{
				real32 value = glm::dot(m_vertices[i], d);
				if (value > bestValue)
				{
					bestIndex = i;
					bestValue = value;
				}
			}

			return bestIndex;
		}

		inline const glm::vec2& DistanceProxy::GetSupportVertex(const glm::vec2& d) const
		{
			s32 bestIndex = 0;
			real32 bestValue = glm::dot(m_vertices[0], d);
			for (s32 i = 1; i < m_count; ++i)
			{
				real32 value = glm::dot(m_vertices[i], d);
				if (value > bestValue)
				{
					bestIndex = i;
					bestValue = value;
				}
			}

			return m_vertices[bestIndex];
		}


	}


}