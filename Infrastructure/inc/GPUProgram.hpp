//
// Created by Moustapha on 04/10/2015.
//

#ifndef BREAK_0_1_GPUPROGRAM_HPP
#define BREAK_0_1_GPUPROGRAM_HPP

#include "Globals.hpp"
#include "MemoryLayout.hpp"
#include "GPUResource.hpp"
#include "Asset.hpp"
#include "SamplerState.hpp"
#include "UniformBuffer.hpp"
#include <string>
#include <memory>

namespace Break{
    namespace Infrastructure{

        class Texture;

        struct BREAK_API uniformRow{
            std::string m_blockName;
            u32 m_offset, m_size;
        };

        struct BREAK_API samplersRow{
            u32 m_slot;
            GPU_ISA m_shader;
            std::shared_ptr<SamplerState> m_state;

            samplersRow(){
                m_state = nullptr;
            }

            ~samplersRow(){
                m_state = nullptr;
            }
        };

        class BREAK_API GPUProgram: public GPUResource, public Asset{
        protected:
            ///will hold the uniform blocks id for assigning later
            std::map<std::string, std::shared_ptr<UniformBuffer> > m_uniformBlocks;
            ///holds uniform names
            std::map<std::string, uniformRow> m_uniformsTable;
            ///holds samplers of this shader
            std::map<std::string, samplersRow> m_samplersTable;

            ///shader actual program in GLSL or HLSL
            std::string m_vs, m_ps;
            ///shader layout
            MemoryLayout m_inputLayout;
        public:

            RTTI(GPUProgram);

            /**
			 * \brief init shader constructor
			 * \param vs vertex shader program
			 * \param ps pixel shader program
			 * \param layout shader input layout
			 * \author Moustapha Saad
			 */
            GPUProgram(std::string vs, std::string ps,MemoryLayout inputLayout);
            GPUProgram(const GPUProgram&);
            virtual ~GPUProgram();

            ///binds this shader program to be used and updates uniform buffers
            void use();

            ///sets uniform to uniform block
            void setUniform(std::string name,void* ptr);
            ///binds texture to sampler unit
            void setTexture(std::string sampler,Texture* tex);

            /**
             * \brief register uniform block in the shader
             * \param name name of the uniform block
             * \param size size of the uniform block
             * \param slot uniform block index in the program
             * \param shader type of the shader that this block in
             * \author Moustapha Saad
             */
            void registerUniformBlock(std::string name,unsigned int size, unsigned int slot,GPU_ISA shader);

            /**
             * \brief registers a uniform in uniform block
             * \param name name of the uniform
             * \param blockName name of the uniform block name
             * \param offset offset of the unifrom inside block
             * \param size size of the uniform inside the block
             * \author Moustapha Saad
             */
            void registerUniform(std::string name,std::string blockName,unsigned int offset, unsigned int size);

            /**
             * \brief registers a sampler to this shader
             * \param name name of the sampler
             * \param slot index of this sampler in the shader
             * \param state the actual state that will be registered
             * \param shader type of the shader that this sampler in
             * \author Moustapha Saad
             */
            void registerSampler(std::string name,unsigned int slot, SamplerStatePtr state,GPU_ISA shader);

            ///flushes the uniform block data to GPU
            void flushUniforms();

            ///gets vertex shader code
            std::string getVertexShader();
            ///gets pixel shader code
            std::string getPixelShader();
            ///gets vertex shader input layout
            MemoryLayout getInputLayout();
        };
        typedef std::shared_ptr<GPUProgram> GPUProgramPtr;

    }
}
#endif //BREAK_0_1_GPUPROGRAM_HPP
