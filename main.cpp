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

    Rect x(0,0,10,10);

    Transform xy;

    Engine* engine = Services::getEngine();
    engine->setup(make_shared<TestApplication>(),API::DirectX11);
    engine->join();
    engine->start();
    //e->setup(NULL,API::OpenGL3_3,NULL);
    return 0;
}