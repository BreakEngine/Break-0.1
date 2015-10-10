//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_ISET_HPP
#define BREAK_0_1_ISET_HPP

#include "Globals.hpp"
#include "MemoryLayout.hpp"
#include "RAMBuffer.hpp"
#include "ServiceException.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API ISet{
        public:

            virtual MemoryLayout& getMemoryLayout(){
                throw ServiceException("unimplemented function");
            };

            virtual RAMBufferPtr copyData(){
                throw ServiceException("unimplemented function");
            }

            virtual RAMBuffer* copyHandle(){
                throw ServiceException("unimplemented function");
            }

            virtual u32 count(){
                throw ServiceException("unimplemented function");
            }

        };
    }
}
#endif //BREAK_0_1_ISET_HPP
