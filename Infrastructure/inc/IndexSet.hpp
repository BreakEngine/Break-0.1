//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_INDEXSET_HPP
#define BREAK_0_1_INDEXSET_HPP

#include "Globals.hpp"
#include "Object.hpp"
#include "RAMBuffer.hpp"
#include "ISet.hpp"
#include "MemoryLayout.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API IndexSet: public Object, public ISet{
        protected:
            std::vector<u32> m_data;

        public:

            RTTI(IndexSet);

            IndexSet():Object("IndexSet",IndexSet::Type)
            {

            }

            virtual ~IndexSet(){
                m_data.clear();
            }

            RAMBufferPtr copyData() override{
                if(m_data.size()>0){
                    return RAMBuffer::asBuffer(&this->m_data[0],m_data.size()*sizeof(u32));
                }else{
                    return nullptr;
                }
            }

            RAMBuffer* copyHandle() override{
                if(m_data.size()>0){
                    return RAMBuffer::asHandle(&this->m_data[0],m_data.size()*sizeof(u32));
                }else{
                    return nullptr;
                }
            }

            u32 count() override{
                return m_data.size();
            }

            MemoryLayout& getMemoryLayout() override{
                MemoryLayout x;
                return x;
            }

            void clear(){
                m_data.clear();
            }

            void resize(u32 size){
                m_data.resize(size);
            }

            void reserve(u32 size){
                m_data.reserve(size);
            }

            u32& operator[](u32 ix){
                return m_data[ix];
            }

            void append(u32 v){
                m_data.push_back(v);
            }

            void erase(u32 start_ix, u32 end_ix){
                auto base = m_data.begin();
                m_data.erase(base+start_ix,base+end_ix+1);
            }
        };
    }
}
#endif //BREAK_0_1_INDEXSET_HPP
