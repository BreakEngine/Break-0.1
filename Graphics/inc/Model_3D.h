#pragma once
#include <Globals.hpp>

#include <Material.h>
#include <AssimpModelLoader.h>
#include <Mesh.h>
#include <Node.h>
#include <ShaderHelper.h>
#include "Transform.hpp"

namespace Break
{
	namespace Graphics
	{

		class BREAK_API Model_3D
		{
		public:
			Model_3D(std::string Dir , std::string ModelName,std::string _Tag);
			~Model_3D();

			void DrawModel(Infrastructure::ShaderHelper* _shaderProgram);
			void PlayAnimation();

			Transform *transformation;
			std::string Tag;

		private:


			void koko(aiMesh* aimesh , int indx);

			void LoadTextures(std::string dir);

			Infrastructure::Mesh* FillMeshes(aiMesh* aimesh);

			Infrastructure::Node* FillNodesTree(aiNode* ainode,Infrastructure::Node* parent);

			glm::mat4 aiMat4x4_To_Mat4(aiMatrix4x4 aimat);

			Infrastructure::AssimpModelLoad *Loader;
			
			std::vector<Infrastructure::Mesh*> meshes;
			std::vector<Infrastructure::Material*> textures;
			Infrastructure::Node* rootNode;


		};


	}
}