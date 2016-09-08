#include <iostream>
#include <Infrastructure.hpp>
#include <TestApplication.hpp>
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

//THIS IS ORIGINAL
int main() {
    cout << "Hello, World!" << endl;
    Engine* engine = Services::getEngine();
    engine->setup(make_shared<TestApplication>(), API::OpenGL3_3);
    //engine->setup(NULL,API::OpenGL3_3,NULL);
    //engine->setup(make_shared<NBodyApp>(), API::DirectX11);
    engine->join();
    engine->start();
    //e->setup(NULL,API::OpenGL3_3,NULL);
    return 0;
}
