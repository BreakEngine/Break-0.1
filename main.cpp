#include <iostream>
#include <Graphics.hpp>
#include <Infrastructure.hpp>
#include "TestApplication.hpp"
#include "NBody.h"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;

//THIS IS ORIGINAL
int main() {
    cout << "Hello, World!" << endl;
    Engine* engine = Services::getEngine();
	engine->setMemorySize(100);
	engine->setup(make_shared<TestApplication>(),API::DirectX11);
	//engine->setup(make_shared<NBodyApp>(), API::DirectX11);
    engine->join();
    engine->start();
    //e->setup(NULL,API::OpenGL3_3,NULL);
    return 0;
}