#pragma once
#include <glm/gtc/constants.hpp>

#define NOT_USED(x) ((void)(x))

	// Collision

	/// The maximum number of contact points between two convex shapes. Do
	/// not change this value.
#define maxManifoldPoints	2

	/// The maximum number of vertices on a convex polygon. You cannot increase
	/// this too much because b2BlockAllocator has a maximum object size.
#define maxPolygonVertices	8

	/// This is used to fatten AABBs in the dynamic tree. This allows proxies
	/// to move by a small amount without triggering a tree adjustment.
	/// This is in meters.
#define aabbExtension		0.1f

	/// This is used to fatten AABBs in the dynamic tree. This is used to predict
	/// the future position based on the current displacement.
	/// This is a dimensionless multiplier.
#define aabbMultiplier		2.0f

	/// A small length used as a collision and constraint tolerance. Usually it is
	/// chosen to be numerically significant, but visually insignificant.
#define linearSlop	0.005f

	/// A small angle used as a collision and constraint tolerance. Usually it is
	/// chosen to be numerically significant, but visually insignificant.
#define angularSlop			(2.0f / 180.0f * glm::pi<double>())

	/// The radius of the polygon/edge shape skin. This should not be modified. Making
	/// this smaller means polygons will have an insufficient buffer for continuous collision.
	/// Making it larger may create artifacts for vertex collision.
#define polygonRadius		(2.0f * linearSlop)

	/// Maximum number of sub-steps per contact in continuous physics simulation.
#define maxSubSteps			8


	// Dynamics

	/// Maximum number of contacts to be handled to solve a TOI impact.
#define maxTOIContacts		32

	/// A velocity threshold for elastic collisions. Any collision with a relative linear
	/// velocity below this threshold will be treated as inelastic.
#define velocityThreshold		1.0f

	/// The maximum linear position correction used when solving constraints. This helps to
	/// prevent overshoot.
#define maxLinearCorrection		0.2f

	/// The maximum angular position correction used when solving constraints. This helps to
	/// prevent overshoot.
#define maxAngularCorrection		(8.0f / 180.0f * glm::pi<double>())

	/// The maximum linear velocity of a body. This limit is very large and is used
	/// to prevent numerical problems. You shouldn't need to adjust this.
#define maxTranslation			2.0f
#define maxTranslationSquared	(maxTranslation * maxTranslation)

	/// The maximum angular velocity of a body. This limit is very large and is used
	/// to prevent numerical problems. You shouldn't need to adjust this.
#define maxRotation				(0.5f * glm::pi<double>())
#define maxRotationSquared		(maxRotation * maxRotation)

	/// This scale factor controls how fast overlap is resolved. Ideally this would be 1 so
	/// that overlap is removed in one time step. However using values close to 1 often lead
	/// to overshoot.
#define baumgarte				0.2f
#define toiBaugarte				0.75f


	// Sleep

	/// The time that a body must be still before it will go to sleep.
#define timeToSleep		0.5f

	/// A body cannot sleep if its linear velocity is above this tolerance.
#define linearSleepTolerance	0.01f

	/// A body cannot sleep if its angular velocity is above this tolerance.
#define angularSleepTolerance	(2.0f / 180.0f * glm::pi<double>())