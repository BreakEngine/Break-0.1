//
// Created by Moustapha on 02/10/2015.
//

#include <Services.hpp>
#include "Mouse.hpp"
#include "Engine.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

map<Mouse::Buttons,Mouse::ButtonState> Mouse::m_data = map<Mouse::Buttons,Mouse::ButtonState>();
glm::uvec2 Mouse::m_position = glm::uvec2();

Mouse::Mouse(){
    m_data.clear();
}

Mouse::~Mouse(){
    //Mouse::_data.clear();
}

void Mouse::update(){

}

Mouse::ButtonState Mouse::getButton(Mouse::Buttons b){
    auto it = m_data.find(b);
    if(it == m_data.end())
        return Mouse::ButtonState::State_None;
    return it->second;
}

glm::uvec2 Mouse::getPosition(){
    return m_position;
}

void Mouse::setPosition(glm::uvec2 val){
    Services::getGraphicsDevice()->setCursorPostion(0, 0);
}

