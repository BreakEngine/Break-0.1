#include "Model_3D.h"
#include <exception>
using namespace Break::Graphics;
using namespace Break::Infrastructure;


Model_3D::Model_3D(std::string Dir , std::string ModelName,std::string _Tag)
{

	transformation = new Transform();
	Tag = _Tag;

	Loader = new AssimpModelLoad();
	Loader->LoadModel(Dir+"/"+ModelName);

	Loader->DisplayDefaultStatistics(); 

	////may take some time here..
	LoadTextures(Dir);


	for(int i =0 ; i < Loader->g_pcAsset->pcScene->mNumMeshes; ++i)
	{
		meshes.push_back(FillMeshes(Loader->g_pcAsset->pcScene->mMeshes[i]));
	}

	//generate Tree Of nodes and its transformations.
	rootNode = FillNodesTree(Loader->g_pcAsset->pcScene->mRootNode, 0);




	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->initBuffers();
	}





}



Model_3D::~Model_3D()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		delete meshes[i];
	}
	for (int i = 0; i < textures.size(); i++)
	{
		delete textures[i];
	}
}

void Model_3D::DrawModel(ShaderHelper* _shaderProgram)
{
	rootNode->DrawNode(*_shaderProgram, transformation->getMatrix());
}


Mesh* Model_3D::FillMeshes(aiMesh* aimesh)
{
	Mesh* mesh = new Mesh();

	for (int i = 0; i < aimesh->mNumVertices; i++)
	{

		//vertex data (geomtry) ..

		aiVector3D vv = aimesh->mVertices[i];
		mesh->VertexData.push_back(glm::vec3(vv.x, vv.y, vv.z));

		//tex coords data..

		if(aimesh->GetNumUVChannels() > 0 ) 
		{
			aiVector3D uvv = aimesh->mTextureCoords[0][i];
			mesh->UVData.push_back(glm::vec2(uvv.x, uvv.y));
		}

		//normals data ..

		aiVector3D nv = aimesh->mNormals[i];
		mesh->NormalsData.push_back(glm::vec3(nv.x, nv.y, nv.z));
	}


	for (int i = 0; i < aimesh->mNumFaces; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			mesh->IndicesData.push_back(aimesh->mFaces[i].mIndices[j]);
		}
	}

	if(aimesh->mMaterialIndex <= textures.size() - 1 && textures.size() > 0)
	{
		mesh->Textures = textures[aimesh->mMaterialIndex];
	}

	//try
	//{
	//	mesh->Textures = textures[aimesh->mMaterialIndex];
	//}
	//catch(...)
	//{
	//	throw "Model Textures Problem take a look on model file.";
	//}

	return mesh;

}


void Model_3D::LoadTextures(std::string dir)
{

	Loader->GetModelMaterial(dir,textures);


}


Node* Model_3D::FillNodesTree(aiNode* ainode,Node* parent)
{

	Node* node = new Node();

	node->name = ainode->mName.data;
	node->parent = parent;
	node->transforms = aiMat4x4_To_Mat4(ainode->mTransformation);


	for (int i = 0; i < ainode->mNumMeshes; i++)
	{
		node->meshes.push_back(meshes[ainode->mMeshes[i]]);
	}

	for (int i = 0; i < ainode->mNumChildren; i++)
	{
		node->children.push_back(FillNodesTree(ainode->mChildren[i], node));
	}

	return node;
	 
}


glm::mat4 Model_3D::aiMat4x4_To_Mat4(aiMatrix4x4 aimat)
{
	glm::mat4 gmat;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			// Transpose matrix, as aiMatrix4x4 is column major while opengl is row major.
			gmat[i][j] = aimat[j][i];
		}
	}
	return gmat;
}

void Model_3D::PlayAnimation()
{
	Loader->UpdateAnimation();
	
}