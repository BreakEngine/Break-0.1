//
// Created by Moustapha on 01/10/2015.
//

#ifndef BREAK_0_1_TESTAPPLICATION_HPP
#define BREAK_0_1_TESTAPPLICATION_HPP
#include "Infrastructure/inc/Infrastructure.hpp"
#include <Graphics.hpp>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <Physics.hpp>

#include "EulerCamera.h"
#include <TriangleModel.h>
#include <plane.h>
#include <Cube.h>
#include <Model_3D.h>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;
using namespace Break::Physics;

using namespace std;


class TestApplication: public Application{
public:


	Cube *SkyBox;
	Cube *ground;

	//Cube *box;
	//Cube *box2;
	//Cube *box3;

	Model_3D* objmodel;
	Model_3D* objmodel1;
	Model_3D* objmodel2;
	Model_3D* objmodel3;
	Model_3D* objmodel4;
	Model_3D* objmodel5;


	

	EulerCamera *camera;
	glm::mat4 VP;
	


	glm::vec2 tmppos;



	TestApplication();

	~TestApplication();

	void init() override;

	void loadResources() override;

	void setupScene() override;

	void cleanUp() override;

	void input() override;

	void update(TimeStep time) override;

	void render() override;

};
#endif //BREAK_0_1_TESTAPPLICATION_HPP

TestApplication::TestApplication() : Application() {
	window = std::make_shared<Window>(800,600,"Test");
}

TestApplication::~TestApplication() {

}

void TestApplication::init() {



	camera = new EulerCamera();
	
	//////////////////////////////////////////////////////////////////////////

	// Projection matrix : 
	camera->SetPerspectiveProjection(45.0f,4.0f/3.0f,0.1f,10000.0f);

	// View matrix : 
	camera->Reset(75.0,5.0,70.0,
					0,0,0,
					0,1,0);
	//////////////////////////////////////////////////////////////////////////



	

	


	Application::init();
}

void TestApplication::loadResources() 
{


	Services::getEngine()->setType(Time::LIMITED);



	//p_img = ResourceLoader<Image>::load("res\\tex\\box.png");
	//p_tex = make_shared<Texture2D>(p_img);



	ground = new Cube("res\\tex\\snow.jpg");


	//ground->Transformation.position = glm::vec3(0,-3,0);
	//ground->Transformation.scale = glm::vec3(50,0.1,50);


   // SkyBox = new Cube("res\\tex\\sky.jpg");
	//SkyBox->Transformation->scale = glm::vec3(100,100,100);



	//box = new Cube("res\\tex\\box.png");
	//box->Transformation.position = glm::vec3(0,-2,0);
	//box->Transformation.rotate(glm::vec3(0,1,0),30);


	//box2 = new Cube("res\\tex\\box.png");
	//box2->Transformation.position = glm::vec3(-3,-2,0);
	//box2->Transformation.rotate(glm::vec3(0,1,0),-30);


	//box3 = new Cube("res\\tex\\box.png");
	//box3->Transformation.position = glm::vec3(-2,0,0);



	objmodel = new Model_3D("res\\models\\unity\\FBX 2013","TCF_Demo_Arches.fbx");
	objmodel->transformation->scale = glm::vec3(0.05,0.05,0.05);
	objmodel->transformation->position = glm::vec3(-160,-5,-30);

	objmodel1  = new Model_3D("res\\models\\unity\\FBX 2013","TCF_Demo_Wall.FBX");
	objmodel1->transformation->scale = glm::vec3(0.01,0.01,0.01);
	objmodel1->transformation->position = glm::vec3(0,-5,0);

	objmodel2  = new Model_3D("res\\models\\unity\\FBX 2013","TCF_Demo_Blox.FBX");
	objmodel2->transformation->scale = glm::vec3(0.03,0.03,0.03);
	objmodel2->transformation->position = glm::vec3(12,-5,0);

	objmodel3  = new Model_3D("res\\models\\unity\\FBX 2013","TCF_Demo_Hellephant.fbx");
	objmodel3->transformation->scale = glm::vec3(0.03,0.03,0.03);
	objmodel3->transformation->position = glm::vec3(12,-5,30);
	objmodel3->transformation->rotate(glm::vec3(0,1,0),-90);


	objmodel4  = new Model_3D("res\\models\\unity\\FBX 2013","TCF_Demo_Player.fbx");
	objmodel4->transformation->scale = glm::vec3(0.03,0.03,0.03);
	objmodel4->transformation->position = glm::vec3(25,-5,30);
	objmodel4->transformation->rotate(glm::vec3(0,1,0),180);

	Application::loadResources();
}

void TestApplication::setupScene()
{

	Application::setupScene();
}

void TestApplication::cleanUp() {
	Application::cleanUp();
}

void TestApplication::input() 
{
	auto m_pos = Mouse::getPosition();

	camera->Yaw(( tmppos.x - m_pos.x ) / 100);


	if(Keyboard::getKey(Keyboard::W) == Keyboard::State_Down)
	{
		camera->Walk(0.5);
	}
	if(Keyboard::getKey(Keyboard::S) == Keyboard::State_Down)
	{
		camera->Walk(-0.5);
	}
	if(Keyboard::getKey(Keyboard::D) == Keyboard::State_Down)
	{
		camera->Strafe(0.5);
	}
	if(Keyboard::getKey(Keyboard::A) == Keyboard::State_Down)
	{
		camera->Strafe(-0.5);
	}


	if(Keyboard::getKey(Keyboard::Esc) == Keyboard::State_Down)
	{
		shutdown();
	}


	tmppos = m_pos;
	Application::input();
}

float angle = 0;

void TestApplication::update(TimeStep tick) 
{

	camera->UpdateViewMatrix();
	VP = camera->GetProjectionMatrix() * camera->GetViewMatrix();

	//angle += 0.001;
	//objmodel->transformation->rotate(glm::vec3 (0,1,0), angle);

	Application::update(tick);
}

void TestApplication::render() 
{

	//SkyBox->Draw(VP);
	//ground->Draw(VP);

	//box->Draw(VP);
	//box2->Draw(VP);
	//box3->Draw(VP);


	//printf("camera pos : %f , %f , %f \n",camera->mPosition.x , camera->mPosition.y ,camera->mPosition.z);

	objmodel->DrawModel(VP);
	objmodel1->DrawModel(VP);
	objmodel2->DrawModel(VP);
	objmodel3->DrawModel(VP);
	objmodel4->DrawModel(VP);

	Application::render();
}
