//
// Created by Moustapha on 01/10/2015.
//

#include <Services.hpp>
#include "Application.hpp"

using namespace Break;
using namespace Break::Infrastructure;


Application::Application(){
    window = nullptr;
}

Application::~Application(){
    cleanUp();
    window = nullptr;
}

void Application::init(){

}
void Application::loadResources(){

}
void Application::setupScene(){

}

void Application::cleanUp(){

}

void Application::input(){

}
void Application::update(/*TimeStep time*/){

}
void Application::render(){

}

Window* Application::getWindow(){
    return window.get();
}

void Application::setWindow(WindowPtr val){
    window = val;
}

void Application::shutdown(){
    Services::getEngine()->shutdown();
}