#pragma once
#include "Globals.hpp"
#include "../deps/assimp/include/Importer.hpp"
#include "../deps/assimp/include/ai_assert.h"
#include "../deps/assimp/include/cfileio.h"
#include "../deps/assimp/include/postprocess.h"
#include "../deps/assimp/include/scene.h"
#include "../deps/assimp/include/IOSystem.hpp"
#include "../deps/assimp/include/IOStream.hpp"
#include "../deps/assimp/include/LogStream.hpp"
#include "../deps/assimp/include/DefaultLogger.hpp"


#include "AssetHelper.h"
#include "Material.h"

namespace Break
{
	namespace Infrastructure
	{

		class BREAK_API AssimpModelLoad
		{
		public:
			AssimpModelLoad();
			~AssimpModelLoad();

			void LoadModel(std::string filePath);
			void DisplayAnimList();
			void DisplayDefaultStatistics();
			void GetModelMaterial(std::string dir,std::vector<Infrastructure::Material*>&textures);

			void UpdateAnimation();
			bool PlayAnimation;

			AssetHelper *g_pcAsset;

		private:
			void LoadAsset(std::string filePath);

			 double Current;
			float g_smoothAngle;
			unsigned int ppsteps;

			//statistics..
			float g_fLoadTime;
			unsigned int iNumVert;
			unsigned int iNumFaces;


			void GetNodeCount(aiNode* pcNode, unsigned int* piCnt);
		};


	}
}