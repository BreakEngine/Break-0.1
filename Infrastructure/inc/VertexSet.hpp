//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_VERTEXSET_HPP
#define BREAK_0_1_VERTEXSET_HPP

#include "Globals.hpp"
#include "Object.hpp"
#include "MemoryLayout.hpp"
#include "RAMBuffer.hpp"
#include <vector>
#include "ISet.hpp"

namespace Break{
    namespace Infrastructure{
        template<class T>
        class VertexSet: public Object, public ISet{
        protected:
            ///data list
            std::vector<T> m_data;

        public:
            ///vertex layout
            MemoryLayout declaration;

            RTTI(VertexSet);

            VertexSet(MemoryLayout layout):Object("VertexSet",VertexSet::Type),declaration(layout){

            }

            VertexSet():Object("VertexSet",VertexSet::Type){

            }

            virtual ~VertexSet(){
                m_data.clear();
            }

            RAMBufferPtr copyData() override{
                if(m_data.size()>0){
                    return RAMBuffer::asBuffer(&this->m_data[0],m_data.size()* declaration.getSize());
                }else{
                    return nullptr;
                }
            }

            RAMBuffer* copyHandle() override{
                if(m_data.size()>0){
                    return RAMBuffer::asHandle(&this->m_data[0],m_data.size()* declaration.getSize());
                }else{
                    return nullptr;
                }
            }

            u32 count() override{
                return m_data.size();
            }

            MemoryLayout& getMemoryLayout() override{
                return declaration;
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

            T& operator[](u32 ix){
                return m_data[ix];
            }

            void append(const T& v){
                m_data.push_back(v);
            }

            void erase(u32 start_ix, u32 end_ix){
                auto base = m_data.begin();
                m_data.erase(base+start_ix,base+end_ix+1);
            }

        };
    }
}
#endif //BREAK_0_1_VERTEXSET_HPP
