#pragma once
#include "Globals.hpp"
#include "Mesh.h"
#include "ShaderHelper.h"

namespace Break
{
	namespace Infrastructure
	{
		class BREAK_API Node
		{

		public: 
			std::string name;
			Node* parent;
			std::vector<Node*> children;
			std::vector<Mesh*> meshes;

			glm::mat4 transforms;

			Node();
			~Node();

			void DrawNode(ShaderHelper& shaderProgram, glm::mat4 &arentTransforms);
		};

	}
}