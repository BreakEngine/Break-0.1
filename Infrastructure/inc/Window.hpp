//
// Created by Moustapha on 30/09/2015.
//

#ifndef BREAK_0_1_WINDOW_HPP
#define BREAK_0_1_WINDOW_HPP

#include "Globals.hpp"
#include <memory>

namespace Break{
    namespace Infrastructure {

        /**
         * represents a Window class of an OS
         */
        class BREAK_API Window{

            /**
             * union of handles of different OSes
             */
            void* m_handle;

            ///window width and height
            u32 m_width, m_height;

            ///window title
            std::string m_title;

            ///fullscreen flag
            bool m_fullscreen;

            ///vsync flag
            bool m_vsync;


        public:

            /**
             * /brief constructor for GLFW Window handle
             * /author Moustapha Saad
             */
            explicit Window(void* handle, u32 width=0, u32 height=0, std::string title="")
                    :m_handle(handle), m_width(width), m_height(height),
                     m_title(title)
            {
                m_fullscreen = false;
                m_vsync = false;
            }

            explicit Window(u32 width, u32 height, std::string title)
                    :m_handle(nullptr), m_width(width), m_height(height),
                     m_title(title)
            {
                m_fullscreen = false;
                m_vsync = false;
            }

            Window():m_handle(nullptr){
                m_fullscreen = false;
                m_vsync = false;
            }

            ~Window(){}

            /**
             * /brief gets handle of the native platform window
             * /return temaplate type of the native handle
             * /author Moustapha Saad
             */
            template <typename T>
            T getHandle(){
                return (T)m_handle;
            }

            /**
             * /brief sets handle of the native platform window
             * /param val value of the native window handle
             * /author Moustapha Saad
             */
            template<typename T>
            void setHandle(T val){
                m_handle = val;
            }

            u32 getWidth() const {
                return m_width;
            }

            void setWidth(u32 m_width) {
                this->m_width = m_width;
            }

            u32 getHeight() const {
                return m_height;
            }

            void setHeight(u32 m_height) {
                this->m_height = m_height;
            }

            const std::string &getTitle() const {
                return m_title;
            }

            void setTitle(const std::string &m_title) {
                this->m_title = m_title;
            }

            bool getFullscreen() const{
                return m_fullscreen;
            }

            void setFullscreen(bool val){
                this->m_fullscreen = val;
            }

            bool getVSync()const{
                return m_vsync;
            }

            void setVSync(bool val){
                this->m_vsync = val;
            }
        };
        typedef std::shared_ptr<Window> WindowPtr;
    }
}
#endif //BREAK_0_1_WINDOW_HPP
