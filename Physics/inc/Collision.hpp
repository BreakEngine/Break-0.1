#pragma once
#include <Globals.hpp>
#include <MathUtils.hpp>
#include <glm/common.hpp>
#include "PhysicsGlobals.hpp"
#include "Transform2D.hpp"

namespace Break
{
	namespace Physics
	{

		class BREAK_API Shape;
		class BREAK_API CircleShape;
		class BREAK_API EdgeShape;
		class BREAK_API PolygonShape;


		const u8 nullFeature = UCHAR_MAX;

		//types of point contacts.
		struct BREAK_API ContactFeature
		{
			enum Type
			{
				vertex = 0,
				face = 1
			};

			u8 indexA;		
			u8 indexB;		
			u8 typeA;		
			u8 typeB;
		};

		//for every contact has ID to reach..
		union BREAK_API ContactID
		{
			ContactFeature cf;
			u32 key;					///< Used to quickly compare contact ids.
		};



		/// A manifold point is a contact point belonging to a contact
		/// manifold. It holds details related to the geometry and dynamics
		/// of the contact points.
		/// The local point usage depends on the manifold type:
		/// -e_circles: the local center of circleB
		/// -e_faceA: the local center of cirlceB or the clip point of polygonB
		/// -e_faceB: the clip point of polygonA
		/// This structure is stored across time steps, so we keep it small.
		/// Note: the impulses are used for internal caching and may not
		/// provide reliable contact forces, especially for high speed collisions.
		struct BREAK_API ManifoldPoint
		{
			glm::vec2 localPoint;		///< usage depends on manifold type
			real32 normalImpulse;	///< the non-penetration impulse
			real32 tangentImpulse;	///< the friction impulse
			ContactID id;			///< uniquely identifies a contact point between two shapes
		};



		/// A manifold for two touching convex shapes.
		/// Box2D supports multiple types of contact:
		/// - clip point versus plane with radius
		/// - point versus point with radius (circles)
		/// The local point usage depends on the manifold type:
		/// -e_circles: the local center of circleA
		/// -e_faceA: the center of faceA
		/// -e_faceB: the center of faceB
		/// Similarly the local normal usage:
		/// -e_circles: not used
		/// -e_faceA: the normal on polygonA
		/// -e_faceB: the normal on polygonB
		/// We store contacts in this way so that position correction can
		/// account for movement, which is critical for continuous physics.
		/// All contact scenarios must be expressed in one of these types.
		/// This structure is stored across time steps, so we keep it small.
		struct BREAK_API Manifold
		{
			enum Type
			{
				circles,
				faceA,
				faceB
			};

			ManifoldPoint points[maxManifoldPoints];	///< the points of contact
			glm::vec2 localNormal;								///< not use for Type::e_points
			glm::vec2 localPoint;								///< usage depends on manifold type
			Type type;
			s32 pointCount;								///< the number of manifold points
		};


		/// This is used to compute the current state of a contact manifold.
		struct BREAK_API WorldManifold
		{
			/// Evaluate the manifold with supplied Transform2Ds. This assumes
			/// modest motion from the original state. This does not change the
			/// point count, impulses, etc. The radii must come from the shapes
			/// that generated the manifold.
			void Initialize(const Manifold* manifold,const Transform2D& xfA, real32 radiusA,const Transform2D& xfB, real32 radiusB);

			glm::vec2 normal;								///< world vector pointing from A to B
			glm::vec2 points[maxManifoldPoints];		///< world contact point (point of intersection)
			real32 separations[maxManifoldPoints];	///< a negative value indicates overlap, in meters
		};



		/// This is used for determining the state of contact points.
		enum BREAK_API PointState
		{
			_nullState,		///< point does not exist
			_addState,		///< point was added in the update
			_persistState,	///< point persisted across the update
			_removeState		///< point was removed in the update
		};


		/// Compute the point states given two manifolds. The states pertain to the transition from manifold1
		/// to manifold2. So state1 is either persist or remove while state2 is either add or persist.
		void BREAK_API GetPointStates(PointState state1[maxManifoldPoints], PointState state2[maxManifoldPoints],const Manifold* manifold1, const Manifold* manifold2);

		/// Used for computing contact manifolds.
		struct BREAK_API ClipVertex
		{
			glm::vec2 v;
			ContactID id;
		};

		/// Ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
		struct BREAK_API RayCastInput
		{
			glm::vec2 p1, p2;
			real32 maxFraction;
		};

		/// Ray-cast output data. The ray hits at p1 + fraction * (p2 - p1), where p1 and p2
		/// come from RayCastInput.
		struct BREAK_API RayCastOutput
		{
			glm::vec2 normal;
			real32 fraction;
		};

		/// An axis aligned bounding box.
		struct BREAK_API AABB
		{
			/// Verify that the bounds are sorted.
			bool IsValid() const;

			/// Get the center of the AABB.
			glm::vec2 GetCenter() const
			{
				return 0.5f * (lowerBound + upperBound);
			}

			/// Get the extents of the AABB (half-widths).
			glm::vec2 GetExtents() const
			{
				return 0.5f * (upperBound - lowerBound);
			}

			/// Get the perimeter length
			real32 GetPerimeter() const
			{
				real32 wx = upperBound.x - lowerBound.x;
				real32 wy = upperBound.y - lowerBound.y;
				return 2.0f * (wx + wy);
			}

			/// Combine an AABB into this one.
			void Combine(const AABB& aabb)
			{
				lowerBound = glm::min(lowerBound, aabb.lowerBound);
				upperBound = glm::max(upperBound, aabb.upperBound);
			}

			/// Combine two AABBs into this one.
			void Combine(const AABB& aabb1, const AABB& aab)
			{
				lowerBound = glm::min(aabb1.lowerBound, aab.lowerBound);
				upperBound = glm::max(aabb1.upperBound, aab.upperBound);
			}

			/// Does this aabb contain the provided AABB.
			bool Contains(const AABB& aabb) const
			{
				bool result = true;
				result = result && lowerBound.x <= aabb.lowerBound.x;
				result = result && lowerBound.y <= aabb.lowerBound.y;
				result = result && aabb.upperBound.x <= upperBound.x;
				result = result && aabb.upperBound.y <= upperBound.y;
				return result;
			}

			bool RayCast(RayCastOutput* output, const RayCastInput& input) const;

			glm::vec2 lowerBound;	///< the lower vertex
			glm::vec2 upperBound;	///< the upper vertex
		};

		/// Compute the collision manifold between two circles.
		void BREAK_API CollideCircles(Manifold* manifold,const CircleShape* circleA, const Transform2D& xfA,const CircleShape* circleB, const Transform2D& xfB);

		/// Compute the collision manifold between a polygon and a circle.
		void BREAK_API CollidePolygonAndCircle(Manifold* manifold,const PolygonShape* polygonA, const Transform2D& xfA,const CircleShape* circleB, const Transform2D& xfB);

		/// Compute the collision manifold between two polygons.
		void BREAK_API CollidePolygons(Manifold* manifold,const PolygonShape* polygonA, const Transform2D& xfA,const PolygonShape* polygonB, const Transform2D& xfB);

		/// Compute the collision manifold between an edge and a circle.
		void BREAK_API CollideEdgeAndCircle(Manifold* manifold,const EdgeShape* polygonA, const Transform2D& xfA,const CircleShape* circleB, const Transform2D& xfB);

		/// Compute the collision manifold between an edge and a circle.
		void BREAK_API CollideEdgeAndPolygon(Manifold* manifold,const EdgeShape* edgeA, const Transform2D& xfA,const PolygonShape* circleB, const Transform2D& xfB);

		/// Clipping for contact manifolds.
		s32 BREAK_API ClipSegmentToLine(ClipVertex vOut[2], const ClipVertex vIn[2],const glm::vec2& normal, real32 offset, s32 vertexIndexA);

		/// Determine if two generic shapes overlap.
		bool BREAK_API TestOverlap(const Shape* shapeA, s32 indexA,const Shape* shapeB, s32 indexB,const Transform2D& xfA, const Transform2D& xfB);

		// ---------------- Inline Functions ------------------------------------------
		
		inline bool AABB::IsValid() const
		{
			glm::vec2 d = upperBound - lowerBound;
			bool valid = d.x >= 0.0f && d.y >= 0.0f;
			valid = valid && Infrastructure::MathUtils::IsValid(lowerBound) && Infrastructure::MathUtils::IsValid(upperBound);
			return valid;
		}

		inline bool TestOverlap(const AABB& a, const AABB& b)
		{
			glm::vec2 d1, d2;
			d1 = b.lowerBound - a.upperBound;
			d2 = a.lowerBound - b.upperBound;

			if (d1.x > 0.0f || d1.y > 0.0f)
				return false;

			if (d2.x > 0.0f || d2.y > 0.0f)
				return false;

			return true;
		}

	}
}