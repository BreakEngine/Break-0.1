//
// Created by Moustapha on 02/10/2015.
//

#include "DXKeyboard.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

void DXKeyboard::keyboardDown(unsigned int k){
    switch (k)
    {
        case 0x41:
            m_data.insert(make_pair(Keys::A, KeyState::State_Down));
            break;
        case 0x42:
            m_data.insert(make_pair(Keys::B, KeyState::State_Down));
            break;
        case 0x43:
            m_data.insert(make_pair(Keys::C, KeyState::State_Down));
            break;
        case 0x44:
            m_data.insert(make_pair(Keys::D, KeyState::State_Down));
            break;
        case 0x45:
            m_data.insert(make_pair(Keys::E, KeyState::State_Down));
            break;
        case 0x46:
            m_data.insert(make_pair(Keys::F, KeyState::State_Down));
            break;
        case 0x47:
            m_data.insert(make_pair(Keys::G, KeyState::State_Down));
            break;
        case 0x48:
            m_data.insert(make_pair(Keys::H, KeyState::State_Down));
            break;
        case 0x49:
            m_data.insert(make_pair(Keys::I, KeyState::State_Down));
            break;
        case 0x4A:
            m_data.insert(make_pair(Keys::J, KeyState::State_Down));
            break;
        case 0x4B:
            m_data.insert(make_pair(Keys::K, KeyState::State_Down));
            break;
        case 0x4C:
            m_data.insert(make_pair(Keys::L, KeyState::State_Down));
            break;
        case 0x4D:
            m_data.insert(make_pair(Keys::M, KeyState::State_Down));
            break;
        case 0x4E:
            m_data.insert(make_pair(Keys::N, KeyState::State_Down));
            break;
        case 0x4F:
            m_data.insert(make_pair(Keys::O, KeyState::State_Down));
            break;
        case 0x50:
            m_data.insert(make_pair(Keys::P, KeyState::State_Down));
            break;
        case 0x51:
            m_data.insert(make_pair(Keys::Q, KeyState::State_Down));
            break;
        case 0x52:
            m_data.insert(make_pair(Keys::R, KeyState::State_Down));
            break;
        case 0x53:
            m_data.insert(make_pair(Keys::S, KeyState::State_Down));
            break;
        case 0x54:
            m_data.insert(make_pair(Keys::T, KeyState::State_Down));
            break;
        case 0x55:
            m_data.insert(make_pair(Keys::U, KeyState::State_Down));
            break;
        case 0x56:
            m_data.insert(make_pair(Keys::V, KeyState::State_Down));
            break;
        case 0x57:
            m_data.insert(make_pair(Keys::W, KeyState::State_Down));
            break;
        case 0x58:
            m_data.insert(make_pair(Keys::X, KeyState::State_Down));
            break;
        case 0x59:
            m_data.insert(make_pair(Keys::Y, KeyState::State_Down));
            break;
        case 0x5A:
            m_data.insert(make_pair(Keys::Z, KeyState::State_Down));
            break;
        case 0x30:
            m_data.insert(make_pair(Keys::Num_0, KeyState::State_Down));
            break;
        case 0x31:
            m_data.insert(make_pair(Keys::Num_1, KeyState::State_Down));
            break;
        case 0x32:
            m_data.insert(make_pair(Keys::Num_2, KeyState::State_Down));
            break;
        case 0x33:
            m_data.insert(make_pair(Keys::Num_3, KeyState::State_Down));
            break;
        case 0x34:
            m_data.insert(make_pair(Keys::Num_4, KeyState::State_Down));
            break;
        case 0x35:
            m_data.insert(make_pair(Keys::Num_5, KeyState::State_Down));
            break;
        case 0x36:
            m_data.insert(make_pair(Keys::Num_6, KeyState::State_Down));
            break;
        case 0x37:
            m_data.insert(make_pair(Keys::Num_7, KeyState::State_Down));
            break;
        case 0x38:
            m_data.insert(make_pair(Keys::Num_8, KeyState::State_Down));
            break;
        case 0x39:
            m_data.insert(make_pair(Keys::Num_9, KeyState::State_Down));
            break;
        case 0x1B:
            m_data.insert(make_pair(Keys::Esc, KeyState::State_Down));
            break;
        case 0xBD:
            m_data.insert(make_pair(Keys::Minus, KeyState::State_Down));
            m_data.insert(make_pair(Keys::Underscore, KeyState::State_Down));
            break;
        case 0xBC:
            m_data.insert(make_pair(Keys::Comma, KeyState::State_Down));
            m_data.insert(make_pair(Keys::Less, KeyState::State_Down));
            break;
        case 0xBB:
            m_data.insert(make_pair(Keys::Plus, KeyState::State_Down));
            m_data.insert(make_pair(Keys::Equals, KeyState::State_Down));
            break;
        case 0xBE:
            m_data.insert(make_pair(Keys::Period, KeyState::State_Down));
            m_data.insert(make_pair(Keys::Greater, KeyState::State_Down));
            break;
        case 0x20:
            m_data.insert(make_pair(Keys::Space, KeyState::State_Down));
            break;
        case 0x0D:
            m_data.insert(make_pair(Keys::Enter, KeyState::State_Down));
            break;
        case 0x08:
            m_data.insert(make_pair(Keys::Backspace, KeyState::State_Down));
            break;
        case 0x09:
            m_data.insert(make_pair(Keys::Tab, KeyState::State_Down));
            break;
        case 0xBF:
            m_data.insert(make_pair(Keys::Question, KeyState::State_Down));
            break;
        case 0xE2:
            m_data.insert(make_pair(Keys::Backslash, KeyState::State_Down));
            break;
        case 0x2E:
            m_data.insert(make_pair(Keys::Delete, KeyState::State_Down));
            break;
        case 0xA0:
            m_data.insert(make_pair(Keys::Left_Shift, KeyState::State_Down));
            break;
        case 0xA1:
            m_data.insert(make_pair(Keys::Right_Shift, KeyState::State_Down));
            break;
        case 0x12:
            m_data.insert(make_pair(Keys::Left_Alt, KeyState::State_Down));
            m_data.insert(make_pair(Keys::Right_Alt, KeyState::State_Down));
            break;
        case 0xA2:
            m_data.insert(make_pair(Keys::Left_Ctrl, KeyState::State_Down));
            break;
        case 0xA3:
            m_data.insert(make_pair(Keys::Right_Ctrl, KeyState::State_Down));
            break;
        case 0x23:
            m_data.insert(make_pair(Keys::End, KeyState::State_Down));
            break;
        case 0x24:
            m_data.insert(make_pair(Keys::Home, KeyState::State_Down));
            break;
        case 0x2D:
            m_data.insert(make_pair(Keys::Insert, KeyState::State_Down));
            break;
        case 0x21:
            m_data.insert(make_pair(Keys::PageUp, KeyState::State_Down));
            break;
        case 0x22:
            m_data.insert(make_pair(Keys::PageDown, KeyState::State_Down));
            break;
        case 0x26:
            m_data.insert(make_pair(Keys::Up, KeyState::State_Down));
            break;
        case 0x28:
            m_data.insert(make_pair(Keys::Down, KeyState::State_Down));
            break;
        case 0x27:
            m_data.insert(make_pair(Keys::Right, KeyState::State_Down));
            break;
        case 0x25:
            m_data.insert(make_pair(Keys::Left, KeyState::State_Down));
            break;
        case 0x70:
            m_data.insert(make_pair(Keys::F1, KeyState::State_Down));
            break;
        case 0x71:
            m_data.insert(make_pair(Keys::F2, KeyState::State_Down));
            break;
        case 0x72:
            m_data.insert(make_pair(Keys::F3, KeyState::State_Down));
            break;
        case 0x73:
            m_data.insert(make_pair(Keys::F4, KeyState::State_Down));
            break;
        case 0x74:
            m_data.insert(make_pair(Keys::F5, KeyState::State_Down));
            break;
        case 0x75:
            m_data.insert(make_pair(Keys::F6, KeyState::State_Down));
            break;
        case 0x76:
            m_data.insert(make_pair(Keys::F7, KeyState::State_Down));
            break;
        case 0x77:
            m_data.insert(make_pair(Keys::F8, KeyState::State_Down));
            break;
        case 0x78:
            m_data.insert(make_pair(Keys::F9, KeyState::State_Down));
            break;
        case 0x79:
            m_data.insert(make_pair(Keys::F10, KeyState::State_Down));
            break;
        case 0x7A:
            m_data.insert(make_pair(Keys::F11, KeyState::State_Down));
            break;
        case 0x7B:
            m_data.insert(make_pair(Keys::F12, KeyState::State_Down));
            break;
        default:
            break;
    }
}
void DXKeyboard::keyboardUp(unsigned int k){
    switch (k)
    {
        case 0x41:
            m_data.insert(make_pair(Keys::A, KeyState::State_Up));
            break;
        case 0x42:
            m_data.insert(make_pair(Keys::B, KeyState::State_Up));
            break;
        case 0x43:
            m_data.insert(make_pair(Keys::C, KeyState::State_Up));
            break;
        case 0x44:
            m_data.insert(make_pair(Keys::D, KeyState::State_Up));
            break;
        case 0x45:
            m_data.insert(make_pair(Keys::E, KeyState::State_Up));
            break;
        case 0x46:
            m_data.insert(make_pair(Keys::F, KeyState::State_Up));
            break;
        case 0x47:
            m_data.insert(make_pair(Keys::G, KeyState::State_Up));
            break;
        case 0x48:
            m_data.insert(make_pair(Keys::H, KeyState::State_Up));
            break;
        case 0x49:
            m_data.insert(make_pair(Keys::I, KeyState::State_Up));
            break;
        case 0x4A:
            m_data.insert(make_pair(Keys::J, KeyState::State_Up));
            break;
        case 0x4B:
            m_data.insert(make_pair(Keys::K, KeyState::State_Up));
            break;
        case 0x4C:
            m_data.insert(make_pair(Keys::L, KeyState::State_Up));
            break;
        case 0x4D:
            m_data.insert(make_pair(Keys::M, KeyState::State_Up));
            break;
        case 0x4E:
            m_data.insert(make_pair(Keys::N, KeyState::State_Up));
            break;
        case 0x4F:
            m_data.insert(make_pair(Keys::O, KeyState::State_Up));
            break;
        case 0x50:
            m_data.insert(make_pair(Keys::P, KeyState::State_Up));
            break;
        case 0x51:
            m_data.insert(make_pair(Keys::Q, KeyState::State_Up));
            break;
        case 0x52:
            m_data.insert(make_pair(Keys::R, KeyState::State_Up));
            break;
        case 0x53:
            m_data.insert(make_pair(Keys::S, KeyState::State_Up));
            break;
        case 0x54:
            m_data.insert(make_pair(Keys::T, KeyState::State_Up));
            break;
        case 0x55:
            m_data.insert(make_pair(Keys::U, KeyState::State_Up));
            break;
        case 0x56:
            m_data.insert(make_pair(Keys::V, KeyState::State_Up));
            break;
        case 0x57:
            m_data.insert(make_pair(Keys::W, KeyState::State_Up));
            break;
        case 0x58:
            m_data.insert(make_pair(Keys::X, KeyState::State_Up));
            break;
        case 0x59:
            m_data.insert(make_pair(Keys::Y, KeyState::State_Up));
            break;
        case 0x5A:
            m_data.insert(make_pair(Keys::Z, KeyState::State_Up));
            break;
        case 0x30:
            m_data.insert(make_pair(Keys::Num_0, KeyState::State_Up));
            break;
        case 0x31:
            m_data.insert(make_pair(Keys::Num_1, KeyState::State_Up));
            break;
        case 0x32:
            m_data.insert(make_pair(Keys::Num_2, KeyState::State_Up));
            break;
        case 0x33:
            m_data.insert(make_pair(Keys::Num_3, KeyState::State_Up));
            break;
        case 0x34:
            m_data.insert(make_pair(Keys::Num_4, KeyState::State_Up));
            break;
        case 0x35:
            m_data.insert(make_pair(Keys::Num_5, KeyState::State_Up));
            break;
        case 0x36:
            m_data.insert(make_pair(Keys::Num_6, KeyState::State_Up));
            break;
        case 0x37:
            m_data.insert(make_pair(Keys::Num_7, KeyState::State_Up));
            break;
        case 0x38:
            m_data.insert(make_pair(Keys::Num_8, KeyState::State_Up));
            break;
        case 0x39:
            m_data.insert(make_pair(Keys::Num_9, KeyState::State_Up));
            break;
        case 0x1B:
            m_data.insert(make_pair(Keys::Esc, KeyState::State_Up));
            break;
        case 0xBD:
            m_data.insert(make_pair(Keys::Minus, KeyState::State_Up));
            m_data.insert(make_pair(Keys::Underscore, KeyState::State_Up));
            break;
        case 0xBC:
            m_data.insert(make_pair(Keys::Comma, KeyState::State_Up));
            m_data.insert(make_pair(Keys::Less, KeyState::State_Up));
            break;
        case 0xBB:
            m_data.insert(make_pair(Keys::Plus, KeyState::State_Up));
            m_data.insert(make_pair(Keys::Equals, KeyState::State_Up));
            break;
        case 0xBE:
            m_data.insert(make_pair(Keys::Period, KeyState::State_Up));
            m_data.insert(make_pair(Keys::Greater, KeyState::State_Up));
            break;
        case 0x20:
            m_data.insert(make_pair(Keys::Space, KeyState::State_Up));
            break;
        case 0x0D:
            m_data.insert(make_pair(Keys::Enter, KeyState::State_Up));
            break;
        case 0x08:
            m_data.insert(make_pair(Keys::Backspace, KeyState::State_Up));
            break;
        case 0x09:
            m_data.insert(make_pair(Keys::Tab, KeyState::State_Up));
            break;
        case 0xBF:
            m_data.insert(make_pair(Keys::Question, KeyState::State_Up));
            break;
        case 0xE2:
            m_data.insert(make_pair(Keys::Backslash, KeyState::State_Up));
            break;
        case 0x2E:
            m_data.insert(make_pair(Keys::Delete, KeyState::State_Up));
            break;
        case 0xA0:
            m_data.insert(make_pair(Keys::Left_Shift, KeyState::State_Up));
            break;
        case 0xA1:
            m_data.insert(make_pair(Keys::Right_Shift, KeyState::State_Up));
            break;
        case 0x12:
            m_data.insert(make_pair(Keys::Left_Alt, KeyState::State_Up));
            m_data.insert(make_pair(Keys::Right_Alt, KeyState::State_Up));
            break;
        case 0xA2:
            m_data.insert(make_pair(Keys::Left_Ctrl, KeyState::State_Up));
            break;
        case 0xA3:
            m_data.insert(make_pair(Keys::Right_Ctrl, KeyState::State_Up));
            break;
        case 0x23:
            m_data.insert(make_pair(Keys::End, KeyState::State_Up));
            break;
        case 0x24:
            m_data.insert(make_pair(Keys::Home, KeyState::State_Up));
            break;
        case 0x2D:
            m_data.insert(make_pair(Keys::Insert, KeyState::State_Up));
            break;
        case 0x21:
            m_data.insert(make_pair(Keys::PageUp, KeyState::State_Up));
            break;
        case 0x22:
            m_data.insert(make_pair(Keys::PageDown, KeyState::State_Up));
            break;
        case 0x26:
            m_data.insert(make_pair(Keys::Up, KeyState::State_Up));
            break;
        case 0x28:
            m_data.insert(make_pair(Keys::Down, KeyState::State_Up));
            break;
        case 0x27:
            m_data.insert(make_pair(Keys::Right, KeyState::State_Up));
            break;
        case 0x25:
            m_data.insert(make_pair(Keys::Left, KeyState::State_Up));
            break;
        case 0x70:
            m_data.insert(make_pair(Keys::F1, KeyState::State_Up));
            break;
        case 0x71:
            m_data.insert(make_pair(Keys::F2, KeyState::State_Up));
            break;
        case 0x72:
            m_data.insert(make_pair(Keys::F3, KeyState::State_Up));
            break;
        case 0x73:
            m_data.insert(make_pair(Keys::F4, KeyState::State_Up));
            break;
        case 0x74:
            m_data.insert(make_pair(Keys::F5, KeyState::State_Up));
            break;
        case 0x75:
            m_data.insert(make_pair(Keys::F6, KeyState::State_Up));
            break;
        case 0x76:
            m_data.insert(make_pair(Keys::F7, KeyState::State_Up));
            break;
        case 0x77:
            m_data.insert(make_pair(Keys::F8, KeyState::State_Up));
            break;
        case 0x78:
            m_data.insert(make_pair(Keys::F9, KeyState::State_Up));
            break;
        case 0x79:
            m_data.insert(make_pair(Keys::F10, KeyState::State_Up));
            break;
        case 0x7A:
            m_data.insert(make_pair(Keys::F11, KeyState::State_Up));
            break;
        case 0x7B:
            m_data.insert(make_pair(Keys::F12, KeyState::State_Up));
            break;
        default:
            break;
    }
}

void DXKeyboard::update(){
    Keyboard::m_data.clear();
}