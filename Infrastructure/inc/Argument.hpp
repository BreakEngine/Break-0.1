//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_ARGUMENT_HPP
#define BREAK_0_1_ARGUMENT_HPP
#include "Globals.hpp"
#include "GPU_ISA.hpp"
#include "GPUHandle.hpp"
#include "MemoryLayout.hpp"
#include "Image.hpp"
#include "Texture.hpp"
#include "SamplerState.hpp"
#include "Object.hpp"
#include "Primitive.hpp"

namespace Break{
    namespace Infrastructure{

        class BREAK_API Arg{
            union
            {
                bool boolean;
                u8 u8_val;
                u16 u16_val;
                u32 u32_val;
                u64 u64_val;

                s8 s8_val;
                s16 s16_val;
                s32 s32_val;
                s64 s64_val;

                real32 real_val;
                real64 real64_val;

                void* ptr;
                GPU_ISA tag;
                GPUHandle* gpu_handle;
                MemoryLayout* mem_layout;
                Image* img;
                Primitive primitive;

                TextureAddressMode address_mode;
                TextureFilter texture_filter;
                CompareFunction compare_function;

                Color* color;

                std::string* str;
            };

        public:


            Arg(bool val):boolean(val){}
            Arg(u8 val):u8_val(val){}
            Arg(u16 val):u16_val(val){}
            Arg(u32 val):u32_val(val){}
            Arg(u64 val):u64_val(val){}

            Arg(s8 val):s8_val(val){}
            Arg(s16 val):s16_val(val){}
            Arg(s32 val):s32_val(val){}
            Arg(s64 val):s64_val(val){}

            Arg(real32 val):real_val(val){}
            Arg(real64 val):real64_val(val){}

            Arg(void* val):ptr(val){}

            Arg(GPU_ISA val):tag(val){}

            Arg(GPUHandle* val):gpu_handle(val){}

            Arg(MemoryLayout* val):mem_layout(val){}

            Arg(std::string* val):str(val){}

            Arg(Image* val):img(val){}

            Arg(Primitive val):primitive(val){}

            Arg(TextureAddressMode val):address_mode(val){}

            Arg(TextureFilter val):texture_filter(val){}

            Arg(CompareFunction val):compare_function(val){}

            Arg(Color* val):color(val){}


            operator bool(){
                return boolean;
            }

            operator u8()
            {
                return u8_val;
            }

            operator u16()
            {
                return u16_val;
            }

            operator u32()
            {
                return u32_val;
            }

            operator u64()
            {
                return u64_val;
            }

            operator s8()
            {
                return s8_val;
            }

            operator s16()
            {
                return s16_val;
            }

            operator s32()
            {
                return s32_val;
            }

            operator s64()
            {
                return s64_val;
            }

            operator real32()
            {
                return real_val;
            }

            operator real64()
            {
                return real64_val;
            }

            operator void*(){
                return ptr;
            }

            operator GPU_ISA(){
                return tag;
            }

            operator GPUHandle*(){
                return gpu_handle;
            }

            operator MemoryLayout*(){
                return mem_layout;
            }

            operator std::string*(){
                return str;
            }

            operator Image*(){
                return img;
            }

            operator Primitive()
            {
                return primitive;
            }

            operator TextureAddressMode()
            {
                return address_mode;
            }

            operator TextureFilter()
            {
                return texture_filter;
            }

            operator CompareFunction()
            {
                return compare_function;
            }

            operator Color*(){
                return color;
            }
        };
    }
}
#endif //BREAK_0_1_ARGUMENT_HPP
