//
// Created by Moustapha on 02/10/2015.
//

#include "MemoryLayout.hpp"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

MemoryLayout::MemoryLayout(){
    m_size = 0;
}

MemoryLayout::MemoryLayout(std::vector<MemoryElement>& e){
    elements = e;
    calcSize();
}

MemoryLayout::MemoryLayout(MemoryElement* e, u32 count){
    for(int i=0;i<count;i++)
        elements.push_back(e[i]);
    calcSize();
}

MemoryLayout::MemoryLayout(const MemoryLayout& val){
    elements = val.elements;
    m_size = val.m_size;
}

u32 MemoryLayout::calcSize(){
    m_size = 0;
    for(auto element : elements){
        m_size += element.size;
    }
    return m_size;
}

void MemoryLayout::append(MemoryElement v){
    if(v.offset == 0)
        v.offset = m_size;
    elements.push_back(v);
    m_size += v.size;
}

u32 MemoryLayout::getSize(){
    return m_size;
}

MemoryLayout::~MemoryLayout(){
    elements.clear();
}

u32 MemoryLayout::getElementCount(){
    return elements.size();
}

bool MemoryLayout::equals(const MemoryLayout& val)
{
    if(m_size != val.m_size)
        return false;

    if(val.elements.size() != elements.size())
        return false;

    for(int i=0;i<elements.size();i++)
        if(!elements[i].equals(val.elements[i])){
            return false;
        }
    return true;
}