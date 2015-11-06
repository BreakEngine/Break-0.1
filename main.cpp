#include <iostream>
#include <Graphics.hpp>
#include <Infrastructure.hpp>
#include "TestApplication.hpp"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;

int main() {
    cout << "Hello, World!" << endl;
    Engine* engine = Services::getEngine();
	engine->setup(make_shared<TestApplication>(),API::OpenGL3_3);
    engine->join();
    engine->start();
    //e->setup(NULL,API::OpenGL3_3,NULL);
    return 0;
}
