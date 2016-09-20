#include "AssimpModelLoader.h"
using namespace Break::Infrastructure;

#include <Windows.h>
//#include <time.h>
//#include <timeapi.h>
#include <cimport.h>


AssimpModelLoad::AssimpModelLoad()
{
	g_smoothAngle = 80.0f;
	g_fLoadTime = 0.0f;
	iNumVert = 0;
	iNumFaces = 0;
	PlayAnimation = true;
	Current = 0;

	// default pp steps
	ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
		aiProcess_JoinIdenticalVertices    | // join identical vertices/ optimize indexing
		aiProcess_ValidateDataStructure    | // perform a full validation of the loader's output
		aiProcess_ImproveCacheLocality     | // improve the cache locality of the output vertices
		aiProcess_RemoveRedundantMaterials | // remove redundant materials
		aiProcess_FindDegenerates          | // remove degenerated polygons from the import
		aiProcess_FindInvalidData          | // detect invalid model data, such as invalid normal vectors
		aiProcess_GenUVCoords              | // convert spherical, cylindrical, box and planar mapping to proper UVs
		aiProcess_TransformUVCoords        | // preprocess UV transformations (scaling, translation ...)
		aiProcess_FindInstances            | // search for instanced meshes and remove them by references to one master
		aiProcess_LimitBoneWeights         | // limit bone weights to 4 per vertex
		aiProcess_OptimizeMeshes		   | // join small meshes, if possible;
		aiProcess_SplitByBoneCount         | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
		0;

}


AssimpModelLoad::~AssimpModelLoad()
{
}


void AssimpModelLoad::LoadModel(std::string filePath)
{

	g_pcAsset = new AssetHelper(); 
	LoadAsset(filePath);

	// create animator
	g_pcAsset->mAnimator = new SceneAnimator( g_pcAsset->pcScene);


	//build asset data vertex , normals , textures..


	//check for animations..
	if (!g_pcAsset->pcScene->HasAnimations()) 
	{
		printf("this model has no animations!! \n");
	}
	else
	{
		DisplayAnimList();
	}


}
void AssimpModelLoad::LoadAsset(std::string filePath)
{
	//double fCur = (double)timeGetTime();

	aiPropertyStore* props = aiCreatePropertyStore();
	aiSetImportPropertyInteger(props,AI_CONFIG_IMPORT_TER_MAKE_UVS,1);
	aiSetImportPropertyFloat(props,AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE,g_smoothAngle);
	aiSetImportPropertyInteger(props,AI_CONFIG_PP_SBP_REMOVE,false ? aiPrimitiveType_LINE | aiPrimitiveType_POINT : 0 );

	aiSetImportPropertyInteger(props,AI_CONFIG_GLOB_MEASURE_TIME,1);


	// Call ASSIMPs C-API to load the file
	g_pcAsset->pcScene = (aiScene*)aiImportFileExWithProperties(filePath.c_str(),
		ppsteps | /* configurable pp steps */
		aiProcess_GenSmoothNormals		   | // generate smooth normal vectors if not existing
		aiProcess_SplitLargeMeshes         | // split large, unrenderable meshes into submeshes
		aiProcess_Triangulate			   | // triangulate polygons with more than 3 edges
		aiProcess_ConvertToLeftHanded	   | // convert everything to D3D left handed space
		aiProcess_SortByPType              | // make 'clean' meshes which consist of a single typ of primitives
		0,
		NULL,
		props);

	aiReleasePropertyStore(props);



	// get the end time of zje operation, calculate delta t
	//double fEnd = (double)timeGetTime();
	//g_fLoadTime = (float)((fEnd - fCur) / 1000);


	if (NULL == g_pcAsset->pcScene)
	{
		printf("[ERROR] Unable to load this asset: %s \n",aiGetErrorString());
	}




}


void  AssimpModelLoad::DisplayAnimList()
{

	if (0 != g_pcAsset->pcScene->mNumAnimations)
	{
		// now fill in all animation names
		for (unsigned int i = 0; i < g_pcAsset->pcScene->mNumAnimations;++i)   
		{
			printf( "%s \n",  g_pcAsset->pcScene->mAnimations[i]->mName.C_Str() );
		}
	}
}


void AssimpModelLoad::GetNodeCount(aiNode* pcNode, unsigned int* piCnt)
{
	*piCnt = *piCnt+1;
	for (unsigned int i = 0; i < pcNode->mNumChildren;++i)
		GetNodeCount(pcNode->mChildren[i],piCnt);
}

void AssimpModelLoad::DisplayDefaultStatistics()
{

	for (unsigned int i = 0; i < g_pcAsset->pcScene->mNumMeshes;++i)
	{
		iNumVert += g_pcAsset->pcScene->mMeshes[i]->mNumVertices;
		iNumFaces += g_pcAsset->pcScene->mMeshes[i]->mNumFaces;
	}

	printf("number of Vertices : %i \n" ,iNumVert );
	printf("number of Faces : %i \n" ,iNumFaces );
	printf("number of Materials : %i \n" ,g_pcAsset->pcScene->mNumMaterials );
	printf("number of Meshes : %i \n" ,g_pcAsset->pcScene->mNumMeshes );

	int z = 0;
	for(int i=0; i < g_pcAsset->pcScene->mNumMaterials ; ++i)
	{
		z += g_pcAsset->pcScene->mMaterials[i]->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE);

	}
	printf("number of Textures Diffuse : %i \n" ,z);


	iNumVert = 0;
	GetNodeCount(g_pcAsset->pcScene->mRootNode,&iNumVert);
	printf("number of Nodes : %i \n" ,iNumVert);

	//printf("Load Time = %.5f \n" ,g_fLoadTime);

}

void AssimpModelLoad::GetModelMaterial(std::string dir,std::vector<Material*>&textures)
{
	aiString path;
	std::string p = "/";
	for(int i =0 ; i < g_pcAsset->pcScene->mNumMaterials ; ++i)
	{
		if(g_pcAsset->pcScene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			path = dir + p + path.C_Str();
			printf("Loading DIFFUSE Texture: %s\n", path.C_Str());

			textures.push_back(new Material(path.C_Str(), 1));
		}

		if(g_pcAsset->pcScene->mMaterials[i]->GetTexture(aiTextureType_AMBIENT, 0, &path) == AI_SUCCESS)
		{
			path = dir + p + path.C_Str();

			printf("Loading AMBIENT Texture: %s\n", path.C_Str());

			textures.push_back(new Material(path.C_Str(), 0));
		}

		if(g_pcAsset->pcScene->mMaterials[i]->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
		{
			path = dir + p + path.C_Str();
			printf("Loading SPECULAR Texture: %s\n", path.C_Str());

			textures.push_back(new Material(path.C_Str(), 0));
		}

		if(g_pcAsset->pcScene->mMaterials[i]->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS)
		{
			path = dir + p + path.C_Str();
			printf("Loading EMISSIVE Texture: %s\n", path.C_Str());

			textures.push_back(new Material(path.C_Str(), 0));
		}

		if(g_pcAsset->pcScene->mMaterials[i]->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
		{
			path = dir + p + path.C_Str();
			printf("Loading HEIGHT Texture: %s\n", path.C_Str());

			textures.push_back(new Material(path.C_Str(), 0));
		}

		if(g_pcAsset->pcScene->mMaterials[i]->GetTexture(aiTextureType_LIGHTMAP, 0, &path) == AI_SUCCESS)
		{
			path = dir + p + path.C_Str();
			printf("Loading LIGHTMAP Texture: %s\n", path.C_Str());

			textures.push_back(new Material(path.C_Str(), 0));
		}
		if(g_pcAsset->pcScene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
		{
			path = dir + p + path.C_Str();
			printf("Loading NORMALS Texture: %s\n", path.C_Str());

			textures.push_back(new Material(path.C_Str(), 0));
		}
		if(g_pcAsset->pcScene->mMaterials[i]->GetTexture(aiTextureType_OPACITY, 0, &path) == AI_SUCCESS)
		{
			path = dir + p + path.C_Str();
			printf("Loading OPACITY Texture: %s\n", path.C_Str());

			textures.push_back(new Material(path.C_Str(), 0));
		}
		if(g_pcAsset->pcScene->mMaterials[i]->GetTexture(aiTextureType_REFLECTION, 0, &path) == AI_SUCCESS)
		{
			path = dir + p + path.C_Str();
			printf("Loading REFLECTION Texture: %s\n", path.C_Str());

			textures.push_back(new Material(path.C_Str(), 0));
		}
		if(g_pcAsset->pcScene->mMaterials[i]->GetTexture(aiTextureType_SHININESS, 0, &path) == AI_SUCCESS)
		{
			path = dir + p + path.C_Str();
			printf("Loading SHININESS Texture: %s\n", path.C_Str());

			textures.push_back(new Material(path.C_Str(), 0));
		}
		if(g_pcAsset->pcScene->mMaterials[i]->GetTexture(aiTextureType_UNKNOWN, 0, &path) == AI_SUCCESS)
		{
			path = dir + p + path.C_Str();
			printf("Loading Texture unknown_Type: %s\n", path.C_Str());

			textures.push_back(new Material(path.C_Str(), 0));
		}

	}
}

void AssimpModelLoad::UpdateAnimation()
{

	//not figured yet...

	//// update possible animation
	//if( g_pcAsset)
	//{
	//	static double lastPlaying = 0.;

	//	ai_assert( g_pcAsset->mAnimator);
	//	if (PlayAnimation) 
	//	{
	//		Current += clock()/ double( CLOCKS_PER_SEC)   -lastPlaying;

	//		double time = Current;
	//		g_pcAsset->mAnimator->SetAnimIndex(1);
	//		aiAnimation* mAnim = g_pcAsset->mAnimator->CurrentAnim();
	//		if(  mAnim && mAnim->mDuration > 0.0) {
	//			double tps = mAnim->mTicksPerSecond ? mAnim->mTicksPerSecond : 25.f;
	//			time = fmod( time, mAnim->mDuration/tps);
	//			//SendDlgItemMessage(g_hDlg,IDC_SLIDERANIM,TBM_SETPOS,TRUE,LPARAM(10000 * (time/(mAnim->mDuration/tps))));
	//		}

	//		g_pcAsset->mAnimator->Calculate( time );
	//		lastPlaying = Current;
	//	}
	//}

	//Sleep(10);
	
}