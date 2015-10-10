//
// Created by Moustapha on 02/10/2015.
//

#include "GLMouse.hpp"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;


void GLMouse::mouseFunc(GLFWwindow* window, int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_LEFT){
        if(action == GLFW_PRESS)
            m_data.insert(make_pair(Buttons::Left_Button,ButtonState::State_Down));
        else if(action == GLFW_RELEASE)
            m_data.insert(make_pair(Buttons::Left_Button,ButtonState::State_Up));
    }else if(button == GLFW_MOUSE_BUTTON_RIGHT){
        if(action == GLFW_PRESS)
            m_data.insert(make_pair(Buttons::Right_Button,ButtonState::State_Down));
        else if(action == GLFW_RELEASE)
            m_data.insert(make_pair(Buttons::Right_Button,ButtonState::State_Up));
    }else if(button == GLFW_MOUSE_BUTTON_MIDDLE){
        if(action == GLFW_PRESS)
            m_data.insert(make_pair(Buttons::Middle_Button,ButtonState::State_Down));
        else if(action == GLFW_RELEASE)
            m_data.insert(make_pair(Buttons::Middle_Button,ButtonState::State_Up));
    }

}

void GLMouse::update(){
    m_data.clear();
}

void GLMouse::mouseMotion(GLFWwindow* window,double x, double y){
    m_position.x = x;
    m_position.y = y;
}