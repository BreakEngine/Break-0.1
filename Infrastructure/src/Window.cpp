//
// Created by Moustapha on 30/09/2015.
//

#include "Window.hpp"
#include <GLFW/glfw3.h>
#ifdef OS_WINDOWS
#include <windows.h>
#endif

using namespace Break;
using namespace Break::Infrastructure;



namespace Break {
    namespace Infrastructure {

//        #ifdef OS_WINDOWS
//        template<>
//        HWND Window::getHandle<HWND>() {
//            return (HWND)m_handle;
//        }
//
//        template<>
//        void Window::setHandle<HWND>(HWND val){
//            m_handle = val;
//        }
//        #endif
//
//        template<>
//        GLFWwindow* Window::getHandle<GLFWwindow*>(){
//            return (GLFWwindow*)m_handle;
//        }
//
//        template<>
//        void Window::setHandle<GLFWwindow*>(GLFWwindow* val){
//            m_handle = val;
//        }
    }
}
