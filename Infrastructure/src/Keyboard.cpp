//
// Created by Moustapha on 02/10/2015.
//

#include "Keyboard.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

//init keyboard
map<Keyboard::Keys, Keyboard::KeyState> Keyboard::m_data = map<Keyboard::Keys, Keyboard::KeyState>();

Keyboard::Keyboard(){
    m_data.clear();
}

Keyboard::~Keyboard(){
    //_data.clear();
}
Keyboard::KeyState Keyboard::getKey(Keyboard::Keys key){
    auto it = m_data.find(key);
    if (it == m_data.end())
        return Keyboard::KeyState::State_None;
    return it->second;
}

void Keyboard::update(){

}