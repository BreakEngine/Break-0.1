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


#include <Scene3D.h>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;
using namespace Break::Physics;

using namespace std;


class TestApplication: public Application{
public:


	Scene3D *scene;





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

TestApplication::TestApplication() : Application() 
{
	window = std::make_shared<Window>(800,600,"Test");
}

TestApplication::~TestApplication() {

}

void TestApplication::init() {


	scene = new Scene3D();
	scene->AddModel("res\\models\\unity\\FBX 2013","TCF_Demo_Player.fbx","Player");

	scene->FindModel("Player")->transformation->scale = glm::vec3(0.05,0.05,0.05);



	Application::init();
}

void TestApplication::loadResources() 
{




	Application::loadResources();
}

void TestApplication::setupScene()
{

	Application::setupScene();
}

void TestApplication::cleanUp() 
{
	Application::cleanUp();
}

void TestApplication::input() 
{
	scene->Update_Inputs();







	if(Keyboard::getKey(Keyboard::Esc) == Keyboard::State_Down)
	{
		shutdown();
	}
	Application::input();
}

void TestApplication::update(TimeStep tick) 
{




	Application::update(tick);
}

void TestApplication::render() 
{



	scene->Draw_Scene();

	Application::render();
}
