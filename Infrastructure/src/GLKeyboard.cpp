//
// Created by Moustapha on 02/10/2015.
//

#include "GLKeyboard.hpp"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;


void GLKeyboard::update(){
    Keyboard::m_data.clear();
}
void GLKeyboard::keyboardFunc(GLFWwindow* window, int key, int scancode, int action, int mods){
    switch (key)
    {
        case GLFW_KEY_A:
            m_data.insert(make_pair(Keys::A, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_B:
            m_data.insert(make_pair(Keys::B, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_C:
            m_data.insert(make_pair(Keys::C, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_D:
            m_data.insert(make_pair(Keys::D, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_E:
            m_data.insert(make_pair(Keys::E, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F:
            m_data.insert(make_pair(Keys::F, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_G:
            m_data.insert(make_pair(Keys::G, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_H:
            m_data.insert(make_pair(Keys::H, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_I:
            m_data.insert(make_pair(Keys::I, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_J:
            m_data.insert(make_pair(Keys::J, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_K:
            m_data.insert(make_pair(Keys::K, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_L:
            m_data.insert(make_pair(Keys::L, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_M:
            m_data.insert(make_pair(Keys::M, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_N:
            m_data.insert(make_pair(Keys::N, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_O:
            m_data.insert(make_pair(Keys::O, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_P:
            m_data.insert(make_pair(Keys::P, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_Q:
            m_data.insert(make_pair(Keys::Q, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_R:
            m_data.insert(make_pair(Keys::R, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_S:
            m_data.insert(make_pair(Keys::S, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_T:
            m_data.insert(make_pair(Keys::T, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_U:
            m_data.insert(make_pair(Keys::U, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_V:
            m_data.insert(make_pair(Keys::V, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_W:
            m_data.insert(make_pair(Keys::W, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_X:
            m_data.insert(make_pair(Keys::X, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_Y:
            m_data.insert(make_pair(Keys::Y, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_Z:
            m_data.insert(make_pair(Keys::Z, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_0:
            m_data.insert(make_pair(Keys::Num_0, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_1:
            m_data.insert(make_pair(Keys::Num_1, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_2:
            m_data.insert(make_pair(Keys::Num_2, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_3:
            m_data.insert(make_pair(Keys::Num_3, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_4:
            m_data.insert(make_pair(Keys::Num_4, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_5:
            m_data.insert(make_pair(Keys::Num_5, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_6:
            m_data.insert(make_pair(Keys::Num_6, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_7:
            m_data.insert(make_pair(Keys::Num_7, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_8:
            m_data.insert(make_pair(Keys::Num_8, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_9:
            m_data.insert(make_pair(Keys::Num_9, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_ESCAPE:
            m_data.insert(make_pair(Keys::Esc, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_MINUS:
            m_data.insert(make_pair(Keys::Minus, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_EQUAL:
            m_data.insert(make_pair(Keys::Plus, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_PERIOD:
            m_data.insert(make_pair(Keys::Period, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_SPACE:
            m_data.insert(make_pair(Keys::Space, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_ENTER:
            m_data.insert(make_pair(Keys::Enter, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_BACKSPACE:
            m_data.insert(make_pair(Keys::Backspace, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_TAB:
            m_data.insert(make_pair(Keys::Tab, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_COMMA:
            m_data.insert(make_pair(Keys::Less, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_SEMICOLON:
            m_data.insert(make_pair(Keys::Colon, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_SLASH:
            m_data.insert(make_pair(Keys::Question, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_BACKSLASH:
            m_data.insert(make_pair(Keys::Backslash, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_DELETE:
            m_data.insert(make_pair(Keys::Delete, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_LEFT_SHIFT:
            m_data.insert(make_pair(Keys::Left_Shift, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            m_data.insert(make_pair(Keys::Right_Shift, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_LEFT_ALT:
            m_data.insert(make_pair(Keys::Left_Alt, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_RIGHT_ALT:
            m_data.insert(make_pair(Keys::Right_Alt, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_LEFT_CONTROL:
            m_data.insert(make_pair(Keys::Left_Ctrl, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_RIGHT_CONTROL:
            m_data.insert(make_pair(Keys::Right_Ctrl, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_END:
            m_data.insert(make_pair(Keys::End, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_HOME:
            m_data.insert(make_pair(Keys::Home, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_INSERT:
            m_data.insert(make_pair(Keys::Insert, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_PAGE_UP:
            m_data.insert(make_pair(Keys::PageUp, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_PAGE_DOWN:
            m_data.insert(make_pair(Keys::PageDown, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_UP:
            m_data.insert(make_pair(Keys::Up, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_DOWN:
            m_data.insert(make_pair(Keys::Down, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_RIGHT:
            m_data.insert(make_pair(Keys::Right, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_LEFT:
            m_data.insert(make_pair(Keys::Left, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F1:
            m_data.insert(make_pair(Keys::F1, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F2:
            m_data.insert(make_pair(Keys::F2, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F3:
            m_data.insert(make_pair(Keys::F3, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F4:
            m_data.insert(make_pair(Keys::F4, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F5:
            m_data.insert(make_pair(Keys::F5, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F6:
            m_data.insert(make_pair(Keys::F6, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F7:
            m_data.insert(make_pair(Keys::F7, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F8:
            m_data.insert(make_pair(Keys::F8, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F9:
            m_data.insert(make_pair(Keys::F9, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F10:
            m_data.insert(make_pair(Keys::F10, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F11:
            m_data.insert(make_pair(Keys::F11, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        case GLFW_KEY_F12:
            m_data.insert(make_pair(Keys::F12, (action == GLFW_PRESS || action == GLFW_REPEAT) ? KeyState::State_Down : KeyState::State_Up));
            break;
        default:
            break;
    }
}