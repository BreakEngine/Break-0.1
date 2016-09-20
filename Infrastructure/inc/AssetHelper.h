#pragma once
#include "Globals.hpp"

#include "../deps/assimp/include/scene.h"
#include <glm/glm.hpp>

#include "SceneAnimator.h"

namespace Break
{

	namespace Infrastructure
	{

		class BREAK_API AssetHelper
		{
		public:

			// default constructor
			AssetHelper()
			{
				mAnimator = NULL;
				pcScene = NULL;
			}



			// Scene wrapper instance
			aiScene* pcScene;

			// Animation player to animate the scene if necessary
			SceneAnimator* mAnimator;

		};



	}

}
