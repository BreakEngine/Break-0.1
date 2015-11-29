#pragma once
#include "BlockAllocator.hpp"
#include "Globals.hpp"
#include "Collision.hpp"

namespace Break
{

	namespace Physics
	{
		class Transform2D;

		/// This holds the mass data computed for a shape.
		//need to hold all mass data and recive maass from here <<------
		struct BREAK_API MassData
		{
			/// The mass of the shape, usually in kilograms.
			real32 mass;

			/// The position of the shape's centroid relative to the shape's origin.
			glm::vec2 center;

			/// The rotational inertia of the shape about the local origin.
			real32 I;
		};


		/// A shape is used for collision detection. You can create a shape however you like.
		/// Shapes used for simulation in World are created automatically when a Fixture
		/// is created. Shapes may encapsulate a one or more child shapes.
		class BREAK_API Shape
		{
		public:

			enum Type
			{
				circle = 0,
				edge = 1,
				polygon = 2,
				chain = 3,
				typeCount = 4
			};

			virtual ~Shape() {}

			/// Clone the concrete shape using the provided allocator.
			virtual Shape* Clone(BlockAllocator* allocator) const = 0;

			/// Get the type of this shape. You can use this to down cast to the concrete shape.
			/// @return the shape type.
			Type GetType() const;

			/// Get the number of child primitives.
			virtual s32 GetChildCount() const = 0;

			/// Test a point for containment in this shape. This only works for convex shapes.
			/// @param xf the shape world Transform2D.
			/// @param p a point in world coordinates.
			virtual bool TestPoint(const Transform2D& xf, const glm::vec2& p) const = 0;


			/// Cast a ray against a child shape.
			/// @param output the ray-cast results.
			/// @param input the ray-cast input parameters.
			/// @param Transform2D the Transform2D to be applied to the shape.
			/// @param childIndex the child shape index
			virtual bool RayCast(RayCastOutput* output, const RayCastInput& input,const Transform2D& Transform2D, s32 childIndex) const = 0;


			/// Given a Transform2D, compute the associated axis aligned bounding box for a child shape.
			///AABB algorithm <<-----
			/// @param aabb returns the axis aligned box.
			/// @param xf the world Transform2D of the shape.
			/// @param childIndex the child shape
			virtual void ComputeAABB(AABB* aabb, const Transform2D& xf, s32 childIndex) const = 0;



			/// Compute the mass properties of this shape using its dimensions and density.
			/// The inertia tensor is computed about the local origin.
			/// @param massData returns the mass data for this shape.
			/// @param density the density in kilograms per meter squared.
			virtual void ComputeMass(MassData* massData, real32 density) const = 0;

			Type m_type;
			real32 m_radius;
		};

		inline Shape::Type Shape::GetType() const
		{
			return m_type;
		}

	}

}