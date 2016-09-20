#include "Scene3D.h"
#include "Infrastructure.hpp"
using namespace Break::Graphics;
using namespace Break::Infrastructure;

Scene3D::Scene3D()
{
	init_Camera_Mat();

	
	shaderProgram = new ShaderHelper();
	shaderProgram->LoadModelShaderProgram();
	shaderProgram->UseModelShaderProgram();
}

Scene3D::~Scene3D()
{
}

void Scene3D::AddModel(std::string Dir , std::string ModelName,std::string Tag)
{
	Models.push_back(new Model_3D(Dir,ModelName,Tag));
}

Model_3D* Scene3D::FindModel(std::string Tag)
{
	for(int i =0 ; i < Models.size(); ++i)
	{
		if(Models[i]->Tag == Tag)
		{
			return Models[i];
		}
	}
}


void Scene3D::init_Camera_Mat()
{
	default_camera = new FP_Camera();

	// Projection matrix : 
	default_camera->SetPerspectiveProjection(45.0f,4.0f/3.0f,0.1f,10000.0f);

	// View matrix : 
	default_camera->Reset(-5.0,0.0,-5.0,
		0,0,0,
		0,1,0);
}


void Scene3D::Update_Inputs()
{
	auto m_pos = Mouse::getPosition();

	default_camera->Yaw(( tmp_mouse_pos.x - m_pos.x ) / 100);

	if(Keyboard::getKey(Keyboard::W) == Keyboard::State_Down)
	{
		default_camera->Walk(0.5);
	}
	if(Keyboard::getKey(Keyboard::S) == Keyboard::State_Down)
	{
		default_camera->Walk(-0.5);
	}
	if(Keyboard::getKey(Keyboard::D) == Keyboard::State_Down)
	{
		default_camera->Strafe(0.5);
	}
	if(Keyboard::getKey(Keyboard::A) == Keyboard::State_Down)
	{
		default_camera->Strafe(-0.5);
	}

	default_camera->UpdateViewMatrix();
	View_Proj_Matrix = default_camera->GetProjectionMatrix() * default_camera->GetViewMatrix();

	tmp_mouse_pos = m_pos;
}


void Scene3D::Draw_Scene()
{

	shaderProgram->BindVPMatrix(&View_Proj_Matrix[0][0]);

	for(int i =0 ;i < Models.size() ; ++i)
	{
		Models[i]->DrawModel(shaderProgram);
	}

}


void Scene3D::Draw_Scene_with_Camera(glm::mat4 CameraMatrix)
{

	for(int i =0 ;i < Models.size() ; ++i)
	{
		Models[i]->DrawModel(shaderProgram);
	}

}