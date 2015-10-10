//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_UNIFORMBUFFER_HPP
#define BREAK_0_1_UNIFORMBUFFER_HPP

#include <iostream>
#include "Globals.hpp"
#include "GPUResource.hpp"
#include "Object.hpp"
#include "RAMBuffer.hpp"
#include "GPU_ISA.hpp"


namespace Break{
    namespace Infrastructure{
        class BREAK_API UniformBuffer: public GPUResource, public Object{
        protected:
            ///RAMBuffer
            RAMBufferPtr m_buffer;

            ///shader flag
            GPU_ISA m_shader;

            u32 m_slot;

        public:
            RTTI(UniformBuffer);

            UniformBuffer();

            UniformBuffer(u32 size, u32 slot, GPU_ISA type);

            UniformBuffer(const UniformBuffer& val);

            virtual ~UniformBuffer();

            void use();

            u32 getSlot();

            GPU_ISA getShaderType();

            void flush();

            RAMBuffer* getBuffer();

        };
        typedef std::shared_ptr<UniformBuffer> UniformBufferPtr;
    }
}
#endif //BREAK_0_1_UNIFORMBUFFER_HPP
