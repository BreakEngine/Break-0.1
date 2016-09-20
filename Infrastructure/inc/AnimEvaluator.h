#pragma once
#include "Globals.hpp"
#include "../deps/assimp/include/scene.h"
#include <tuple>


namespace Break
{
	namespace Infrastructure
	{


		/** Calculates transformations for a given timestamp from a set of animation tracks. Not directly useful,
		* better use the AnimPlayer class.
		*/
		class BREAK_API AnimEvaluator
		{
		public:
			/** Constructor on a given animation. The animation is fixed throughout the lifetime of
			* the object.
			* @param pAnim The animation to calculate poses for. Ownership of the animation object stays
			*   at the caller, the evaluator just keeps a reference to it as long as it persists.
			*/
			AnimEvaluator( const aiAnimation* pAnim);

			/** Evaluates the animation tracks for a given time stamp. The calculated pose can be retrieved as a
			* array of transformation matrices afterwards by calling GetTransformations().
			* @param pTime The time for which you want to evaluate the animation, in seconds. Will be mapped into the animation cycle, so
			*   it can be an arbitrary value. Best use with ever-increasing time stamps.
			*/
			void Evaluate( double pTime);

			/** Returns the transform matrices calculated at the last Evaluate() call. The array matches the mChannels array of
			* the aiAnimation. */
			const std::vector<aiMatrix4x4>& GetTransformations() const { return mTransforms; }

		protected:
			/** The animation we're working on */
			const aiAnimation* mAnim;

			/** At which frame the last evaluation happened for each channel.
			* Useful to quickly find the corresponding frame for slightly increased time stamps
			*/
			double mLastTime;
			std::vector<std::tuple<unsigned int, unsigned int, unsigned int> > mLastPositions;

			/** The array to store the transformations results of the evaluation */
			std::vector<aiMatrix4x4> mTransforms;
		};



	}
}
