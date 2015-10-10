//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_GPURESOURCE_HPP
#define BREAK_0_1_GPURESOURCE_HPP

#include "Globals.hpp"
#include <memory>
#include "GPUHandle.hpp"

namespace Break{
    namespace Infrastructure{
        class DXDevice;
        class GLDevice;
        class BREAK_API GPUResource{
            friend class DXDevice;
            friend class GLDevice;

        protected:
            ///pointer to handle
            GPUHandlePtr m_handle;
        public:
            GPUResource(){

            }

            GPUResource(const GPUResource& val){
                m_handle = val.m_handle;
            }

            virtual ~GPUResource(){

            }

            GPUHandle* getHandle(){
                if(m_handle)
                    return m_handle.get();
                else
                    return nullptr;
            }

            void setHandle(GPUHandlePtr val){
                m_handle = val;
            }
        };

        typedef std::shared_ptr<GPUResource> GPUResourcePtr;
    }
}
#endif //BREAK_0_1_GPURESOURCE_HPP
