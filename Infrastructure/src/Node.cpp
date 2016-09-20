#include "Node.h"
using namespace Break::Infrastructure;

Node::Node()
{
}

Node::~Node()
{
}

void Node::DrawNode(ShaderHelper& shaderProgram, glm::mat4 &parentTransforms)
{

	glm::mat4 curTransform = parentTransforms * this->transforms;

	shaderProgram.BindModelMatrix(&curTransform[0][0]);

	for (int i = 0; i < meshes.size(); i++)
	{  
		meshes[i]->DrawMesh();
	}
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->DrawNode(shaderProgram, curTransform);
	}
}