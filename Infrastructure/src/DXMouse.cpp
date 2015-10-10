//
// Created by Moustapha on 02/10/2015.
//

#include "DXMouse.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

void DXMouse::mouseButton(int button, int state){
    if(state == 0)
        m_data.insert(make_pair((Buttons)button,ButtonState::State_Down));
    else if(state == 1)
        m_data[(Buttons)button] = ButtonState::State_Up;
    else
        m_data.insert(make_pair((Buttons)button,ButtonState::State_None));
}

void DXMouse::update(){
    m_data.clear();
}
void DXMouse::mouseMove(int x, int y){
    m_position.x = x;
    m_position.y = y;
}